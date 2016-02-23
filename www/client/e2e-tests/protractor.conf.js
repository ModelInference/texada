exports.config = {
    allScriptsTimeout: 11000,

    specs: [
        '*.js'
    ],
    /*
     capabilities: {
        'browserName': 'chrome'
     },
     */
    capabilities: {
        'browserName': 'phantomjs',
        'phantomjs.binary.path': "../node_modules/phantomjs-prebuilt/bin/phantomjs"
    },

    baseUrl: 'http://localhost:8080/texada/',

    framework: 'jasmine',

    jasmineNodeOpts: {
        defaultTimeoutInterval: 30000
    }
};
