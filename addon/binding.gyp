{
    "targets" : [ 
        {
            "target_name" : "addon",
                "sources" : ["addon.cc","kchash.cc"],
                "include_dirs":["/home/yul13/bin/include" ],
                "libraries":[ "-lkyotocabinet", "-L/home/yul13/bin/lib"]
        }
    ]
}
