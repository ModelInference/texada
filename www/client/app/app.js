'use strict';

// Declare app level module which depends on views, and components
angular.module('myApp', [
    'ngRoute',
    'myApp.view1',
    'myApp.view2',
    'myApp.version'
]).
    config(['$routeProvider', function ($routeProvider) {
        $routeProvider.otherwise({
            redirectTo: '/view1'
        });
    }]);

$(document).ready(function () {
    $(document).ready(function () {
        $("#myTab li:eq(1) a").tab('show');
    })
});
