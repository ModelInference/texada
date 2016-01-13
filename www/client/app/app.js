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
    $scope.ltl = "-f 'G(x->XFy)' -l";    // The LTL property to be mined
    $scope.text = "a\nb\nc\n--\nb\nb\nc\na\n--\nc\na\nb\nc\n--";   // The log/data to mine

    $scope.uploadOrText = "text";      // File upload or textbox (textbox used as default)
    $scope.commonPropSelected = "";    // Selected common prop type

    // Common property types to be used
    $scope.commonProps = {
        "always-followed-by": "-f 'G(x->XFy)'",
        "immediately-followed-by\/followedby": "-f 'G(x -> X y)'",
        "perracotta\/alternating": "-f '(!y W x) & G((x -> X(!x U y))&(y -> X(!y W x)))'",
        "perracotta\/causefirst": "-f '(!y W x) & G(x -> XFy)'",
        "perracotta\/effectfirst": "-f 'G((x -> X(!x U y)) & (y -> X(!y W x)))'",
        "perracotta\/multicause": "-f '(!y W x) & G((x -> XFy) & (y -> X(!y W x)))'",
        "perracotta\/multieffect": "-f '(!y W x) & G(x -> X(!x U y))'",
        "perracotta\/onecause": "-f 'G(x -> X(!x U y))'",
        "perracotta\/oneeffect": "-f 'G((x -> XFy) & (y -> X(!y W x)))'",
        "perracotta\/response": "-f 'G(x->XFy)'",
        "synoptic\/always-followed-by": "-f 'G(x -> XF y)'",
        "synoptic\/always-precedes": "-f 'Fy -> (!y U x)'",
        "synoptic\/never-followed-by": "-f 'G(x -> G !y)'",
        "spec-patterns\/absence\/after_q": "-f 'G(q -> G(!p))'",
        "spec-patterns\/absence\/after_q_until_r": "-f 'G(q & !r -> (!p W r))'",
        "spec-patterns\/absence\/before_r": "-f 'Fr -> (!p U r)'",
        "spec-patterns\/absence\/between_q_and_r": "-f 'G((q & !r & Fr) -> (!p U r))'",
        "spec-patterns\/absence\/globally": "-f 'G(!p)'",
        "spec-patterns\/bounded_existence\/after_q": "-f 'Fq -> (!q U (q & (!p W (p W (!p W (p W G!p))))))'",
        "spec-patterns\/bounded_existence\/after_q_until_r": "-f 'G(q -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p W r) | Gp)))))))))'",
        "spec-patterns\/bounded_existence\/before_r": "-f 'Fr -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p U r)))))))))'",
        "spec-patterns\/bounded_existence\/between_q_and_r": "-f 'G((q & Fr) -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p U r))))))))))'",
        "spec-patterns\/bounded_existence\/globally": "-f '(!p W (p W (!p W (p W G!p))))'",
        "spec-patterns\/constrained_chain\/after_q": "-f 'G (q -> G (p -> (s & !z & X(!z U t))))'",
        "spec-patterns\/constrained_chain\/after_q_until_r": "-f 'G (q -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U (r | G (p -> (s & !z & X(!z U t)))))'",
        "spec-patterns\/constrained_chain\/before_r": "-f 'Fr -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r'",
        "spec-patterns\/constrained_chain\/between_q_and_r": "-f 'G ((q & Fr) -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r)'",
        "spec-patterns\/constrained_chain\/globally": "-f 'G (p -> F(s & !z & X(!z U t)))'",
        "spec-patterns\/existence\/after_q": "-f 'G(!q) | F(q & Fp))'",
        "spec-patterns\/existence\/after_q_until_r": "-f 'G(q & !r -> (!r U (p & !r)))'",
        "spec-patterns\/existence\/before_r": "-f '!r W (p & !r)'",
        "spec-patterns\/existence\/between_q_and_r": "-f 'G(q & !r -> (!r W (p & !r)))'",
        "spec-patterns\/existence\/globally": "-f 'F(p)'",
        "spec-patterns\/precedence\/after_q": "-f 'G!q | F(q & (!p W s))'",
        "spec-patterns\/precedence\/after_q_until_r": "-f 'G(q & !r -> (!p W (s | r)))'",
        "spec-patterns\/precedence\/before_r": "-f 'Fr -> (!p U (s | r))'",
        "spec-patterns\/precedence\/between_q_and_r": "-f 'G((q & !r & Fr) -> (!p U (s | r)))'",
        "spec-patterns\/precedence\/globally": "-f '!p W s'",
        "spec-patterns\/response\/after_q": "-f 'G(q -> G(p -> Fs))'",
        "spec-patterns\/response\/after_q_until_r": "-f 'G(q & !r -> ((p -> (!r U (s & !r))) W r)'",
        "spec-patterns\/response\/before_r": "-f 'Fr -> (p -> (!r U (s & !r))) U r'",
        "spec-patterns\/response\/between_q_and_r": "-f 'G((q & !r & Fr) -> (p -> (!r U (s & !r))) U r)'",
        "spec-patterns\/response\/globally": "-f 'G(p -> Fs)'",
        "spec-patterns\/universality\/after_q": "-f 'G(q -> G(p))'",
        "spec-patterns\/universality\/after_q_until_r": "-f 'G(q & !r -> (p W r))'",
        "spec-patterns\/universality\/before_r": "-f 'Fr -> (p U r)'",
        "spec-patterns\/universality\/between_q_and_r": "-f 'G((q & !r & Fr) -> (p U r))'",
        "spec-patterns\/universality\/globally": "-f 'G(p)'"
    };


    $scope.bindings = [];      // The results fetched: the bindings found
    $scope.properties = [];    // The results fetched: the properties mined

    // If args are empty, then disable mine button
    $scope.$watch("ltl", function (value, old) {
        if (value.length < 1) {
            $("#mineButton").attr("disabled", true);
        }
        else {
            if ($scope.uploadOrText == "upload") {
                var uploadValue = $("#file").val();
                if (uploadValue.length > 0) {
                    $("#mineButton").removeAttr("disabled");
                }
            }
            else {
                if ($scope.text.length > 0) {
                    $("#mineButton").removeAttr("disabled");
                }
            }
        }
    });

    // If log is empty, then disable mine button (if textbox is selected)
    $scope.$watch("text", function (value, old) {
        if ($scope.uploadOrText != "upload") {
            if (value.length < 1) {
                $("#mineButton").attr("disabled", true);
            }
            else if ($scope.ltl.length > 0) {
                $("#mineButton").removeAttr("disabled");
            }
        }

    });

    // If upload is selected but no file uploaded, disable mine button
    $scope.$watch("uploadOrText", function (value, old) {
        var uploadValue = $("#file").val();
        var logValue = $scope.text;

        if (value == "upload") {
            $("#inputText").attr("disabled", true);
            $("#file").removeAttr("disabled");

            if (uploadValue.length < 1) {
                $("#mineButton").attr("disabled", true);
            }
            else {
                $("#mineButton").removeAttr("disabled");
            }
        }
        else {
            $("#file").attr("disabled", true);
            $("#inputText").removeAttr("disabled");
            if (logValue.length < 1) {
                $("#mineButton").attr("disabled", true);
            }
            else {
                $("#mineButton").removeAttr("disabled");
            }
        }
        if ($scope.ltl.length < 1) {
            $("#mineButton").attr("disabled", true);
        }
    });

    // Add common property type
    $scope.addCommonProp = function () {
        var commonProp = $scope.commonPropSelected + " -l";
        $scope.ltl = commonProp;
        $("#commonPropsModal").modal("hide");

    };

    // Server returned a 200 OK response
    $scope.miningSuccess = function (data) {

        // Try to parse JSON
        try {
            var parsedData = jQuery.parseJSON(data);
        }
        catch (e) {
            showErrorModal(data);
            $scope.$apply();
            return;
        }

        console.log(parsedData);

        //Emptying previous output
        $scope.bindings = [];
        $scope.properties = [];


        // Making sure there's only one property type
        if (parsedData.length != 1) {
            showErrorModal("Sorry, we support exactly one property type at a time.");
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

        // Show the output tab
        $("#outputBtn").css({display: "block"});

        // Updata/apply new data
        $scope.$apply();

        // Navigate to output tab
        $("#outputBtn").click();


    };

    // Server returned an error
    $scope.miningFailure = function (data) {
        $scope.bindings = [];
        $scope.properties = [];
        showErrorModal(data.responseText);
        $scope.$apply();
        return;
    }

    // Click handler for the Mine button
    $scope.mine = function () {
        // Arguments for the HTTP request
        var in_str = "{\"log\" : \"" + $scope.text.replace(/\n/g, '\\n') + "\", \"args\" : \"" + $scope.ltl.replace(/\n/g, "") + "\"}";

        // Form data (primarily used to retrieve file to be uploaded)
        var formData = new FormData($("#uploadForm")[0]);

        // Decide whether file is being uploaded or not
        if ($scope.uploadOrText == "upload") {
            // Send request to server (upload file)
            $.ajax({
                "method": "POST",
                "url": "/texada/uploadMine/",
                "data": formData,
                cache: false,
                contentType: false,
                processData: false
            }).done($scope.miningSuccess).fail($scope.miningFailure);
        }
        else {
            $.ajax({
                "method": "POST",
                "url": "/texada/mine/",
                "data": in_str
            }).done($scope.miningSuccess).fail($scope.miningFailure);
        }
    }


});
// Tests


// Services


// Document ready
$(document).ready(function () {
    // Activate chosen (the search+dropdown plugin)
    $("#chooseCommon").chosen({width: "100%"});
});

// Functions

// Show the error modal
function showErrorModal(msg) {
    $("#errorMessage").text(msg);
    $("#errorModal").modal();
}

// Called when file is selected
function fileChange() {
    if ($("#argumentsText").val().length > 0) {
        $("#mineButton").removeAttr("disabled");
    }
}



