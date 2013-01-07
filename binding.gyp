{
  "targets": [
    {
      "target_name": "nscws",
      "sources": [ "scws.cc" ],
      "include_dirs" : ["/opt/scws/include"],
      "libraries" : ["-lscws", "-L/opt/scws/lib"],
      "link_settings" : {
        "library_dirs" : ["/opt/scws/lib"]
      }
    }
  ]
}
