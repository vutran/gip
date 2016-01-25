{
  "targets": [
    {
      "target_name": "gip",
      "sources": [ "gip.cc" ],
      "include_dirs":[
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}