#include <node.h>
#include <windows.h>
#include <objidl.h>
#include <string>
#include <shellapi.h>
#include <shlobj.h>
#include <algorithm>
#include <shlwapi.h>
#include <GdiplusFlat.h>

#include <commoncontrols.h>
#include <comip.h>
#include <comdef.h>
namespace Gdiplus
{
  using std::min;
  using std::max;
};
using namespace std;
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib, "shlwapi.lib")
using namespace Gdiplus;

static CLSID g_pngClsid = GUID_NULL;

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms533843(v=vs.85).aspx

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in chars

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

static HICON getShellIconByIndex(int shilsize, int iImage)
{
    IImageList* spiml;
    SHGetImageList(shilsize, IID_PPV_ARGS(&spiml));

    HICON hico;
    spiml->GetIcon(iImage, ILD_TRANSPARENT, &hico);
    return hico;
}

static HICON getShellIcon(int shilsize, const std::wstring& fname) {
    UINT flags =   SHGFI_ICON | SHGFI_LARGEICON;
    SHFILEINFO fi = {0};
    HICON hIcon = NULL;
    HRESULT hr = (HRESULT)SHGetFileInfo((LPCSTR)fname.c_str(), 0, &fi, sizeof(fi), flags);
    return fi.hIcon;
}

struct BITMAP_AND_charS {
    Gdiplus::Bitmap* bmp;
    int32_t* chars;
};

static BITMAP_AND_charS createAlphaChannelBitmapFromIcon(HICON hIcon) {

    // Get the icon info
    ICONINFO iconInfo = {0};
    GetIconInfo(hIcon, &iconInfo);

    // Get the screen DC
    HDC dc = GetDC(NULL);

    // Get icon size info
    BITMAP bm = {0};
    GetObject( iconInfo.hbmColor, sizeof( BITMAP ), &bm );

    // Set up BITMAPINFO
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Extract the color bitmap
    int nBits = bm.bmWidth * bm.bmHeight;
    int32_t* colorBits = new int32_t[nBits];
    GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

    // Check whether the color bitmap has an alpha channel.
        // (On my Windows 7, all file icons I tried have an alpha channel.)
    BOOL hasAlpha = FALSE;
    for (int i = 0; i < nBits; i++) {
        if ((colorBits[i] & 0xff000000) != 0) {
            hasAlpha = TRUE;
            break;
        }
    }

    // If no alpha values available, apply the mask bitmap
    if (!hasAlpha) {
        // Extract the mask bitmap
        int32_t* maskBits = new int32_t[nBits];
        GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
        // Copy the mask alphas into the color bits
        for (int i = 0; i < nBits; i++) {
            if (maskBits[i] == 0) {
                colorBits[i] |= 0xff000000;
            }
        }
        delete[] maskBits;
    } 

    // Release DC and GDI bitmaps
    ReleaseDC(NULL, dc); 
    ::DeleteObject(iconInfo.hbmColor);
    ::DeleteObject(iconInfo.hbmMask); 

    // Create GDI+ Bitmap
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth*4, PixelFormat32bppARGB, (byte*)colorBits);
    BITMAP_AND_charS ret = {bmp, colorBits};
    return ret;
}
static void ISaveFileIconAsPng(HICON hIcon, myBuffer* buf) {
  BITMAP_AND_charS bbs = createAlphaChannelBitmapFromIcon(hIcon);
  IStream* strm = SHCreateMemStream(NULL, NULL);
  bbs.bmp->Save(strm, &g_pngClsid , NULL);
  ULARGE_INTEGER size;
  IStream_Size(strm, &size);
  ULONG charSaved = 0;
  buf->data = new char[(unsigned int)size.QuadPart];
  LARGE_INTEGER li = {0};
  ULARGE_INTEGER uli = {0};
  strm->Seek(li, 0, &uli);
  strm->Read(buf->data, (ULONG)size.QuadPart, &charSaved);
  strm->Release();
  delete bbs.bmp;
  delete[] bbs.chars;
  DestroyIcon(hIcon);
  buf->size = charSaved;
}

static void saveFileIconAsPng(HICON hIcon, myBuffer* buf){
  GdiplusStartupInput gp;
  ULONG_PTR gtoken;
  GdiplusStartup(&gtoken, &gp, NULL);
  GetEncoderClsid(L"image/png", &g_pngClsid);
  ISaveFileIconAsPng(hIcon, buf);
  GdiplusShutdown(gtoken);
  
}