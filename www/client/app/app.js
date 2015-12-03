'use strict';

// Declare app level module which depends on views, and components
var texada = angular.module('texadaApp', [/*
    'ngRoute',
 'texada.view1',
 'texada.view2',
 'texada.0.1.0' */
]);


/*config(['$routeProvider', function ($routeProvider) {
 $routeProvider.otherwise({
 redirectTo: '/view1'
 });
 }]);*/

texada.controller("TexadaHomeCtrl", function ($scope) {
    $scope.ltl = "";    // The LTL property to be mined
    $scope.text = "";   // The log/data to mine
    $scope.par = [1, 2, 3];
    $scope.bindings = [];      // The results fetched: the bindings found
    $scope.properties = [];    // The results fetched: the properties mined

    $scope.mine = function () {
        //$scope.text = $scope.text.replace(/\n/g, '\\n');

        var in_str = "{\"log\" : \"" + $scope.text.replace(/\n/g, '\\n') + "\", \"args\" : \"" + $scope.ltl + "\"}";

        //       var parameters = {"log" : $scope.text.replace(/\n/g, '\\n'), "args" : $scope.ltl};

        $.post("/texada/mine/", in_str, function (data) {

            var parsedData = jQuery.parseJSON(data);
            //Emptying previous output
            $scope.bindings = [];
            $scope.properties = [];


            // Will be edited
            if (parsedData.length != 1) {
                alert("Sorry, there has been an error in mining the property.");
                $scope.$apply();
                return;
            }

            // Variables with the current prop-type and instances
            var instances = parsedData[0]['prop-instances'];
            var propType = parsedData[0]['prop-type']['str'];    // needs to be changed later when multiple prop-type functionality is added


            // Checking instances array is not empty
            if (instances.length > 0) {

                var vars = [];
                var k = 0;
                var p = 0;
                // Iterating through the data
                console.log(instances);
                for (var key in instances) {

                    vars = [];
                    k = 0; // Count of props set to 0
                    for (var prop in instances[key]['vars']) {
                        vars.push(
                            {
                                "prop": prop,
                                "var": instances[key]['vars'][prop],
                                "count": k
                            });
                        k++;
                    }

                    $scope.bindings.push(vars);
                    $scope.properties.push({value: propType, count: p});

                    p++;
                }
            }

            $scope.$apply();
            $("#outputBtn").click();


        })
    }
});
// Tests


// Services


// Document ready
$(document).ready(function () {
    // Activate tabs
    //$("#myTab li:eq(1) a").tab('show');
});

