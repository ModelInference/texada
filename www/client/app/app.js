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

    $scope.uploadOrText = "text";
    $scope.commonPropSelected = "";
    /*
     // Stripped spaces
     $scope.commonProps = {
     "always-followed-by": "-f 'G(x->XFy)'",
     "immediately-followed-by": "-f 'G(x->X y)'",
     "perracotta\/alternating": "-f '(!yWx)&G((x->X(!xUy))&(y->X(!yWx)))'",
     "perracotta\/causefirst": "-f '(!yWx)&G(x->XFy)'",
     "perracotta\/effectfirst": "-f 'G((x->X(!xUy))&(y->X(!yWx)))'",
     "perracotta\/multicause": "-f '(!yWx)&G((x->XFy)&(y->X(!yWx)))'",
     "perracotta\/multieffect": "-f '(!yWx)&G(x->X(!xUy))'",
     "perracotta\/onecause": "-f 'G(x->X(!xUy))'",
     "perracotta\/oneeffect": "-f 'G((x->XFy)&(y->X(!yWx)))'",
     "perracotta\/response": "-f 'G(x->XFy)'",
     "synoptic\/always-followed-by": "-f 'G(x->XFy)'",
     "synoptic\/always-precedes": "-f 'Fy->(!yUx)'",
     "synoptic\/never-followed-by": "-f 'G(x->G!y)'",
     "spec-patterns\/absence\/after_q": "-f 'G(q->G(!p))'",
     "spec-patterns\/absence\/after_q_until_r": "-f 'G(q&!r->(!pWr))'",
     "spec-patterns\/absence\/before_r": "-f 'Fr->(!pUr)'",
     "spec-patterns\/absence\/between_q_and_r": "-f 'G((q&!r&Fr)->(!pUr))'",
     "spec-patterns\/absence\/globally": "-f 'G(!p)'",
     "spec-patterns\/bounded_existence\/after_q": "-f 'Fq->(!qU(q&(!pW(pW(!pW(pWG!p))))))'",
     "spec-patterns\/bounded_existence\/after_q_until_r": "-f 'G(q->((!p&!r)U(r|((p&!r)U(r|((!p&!r)U(r|((p&!r)U(r|(!pWr)|Gp)))))))))'",
     "spec-patterns\/bounded_existence\/before_r": "-f 'Fr->((!p&!r)U(r|((p&!r)U(r|((!p&!r)U(r|((p&!r)U(r|(!pUr)))))))))'",
     "spec-patterns\/bounded_existence\/between_q_and_r": "-f 'G((q&Fr)->((!p&!r)U(r|((p&!r)U(r|((!p&!r)U(r|((p&!r)U(r|(!pUr))))))))))'",
     "spec-patterns\/bounded_existence\/globally": "-f '(!pW(pW(!pW(pWG!p))))'",
     "spec-patterns\/constrained_chain\/after_q": "-f 'G(q->G(p->(s&!z&X(!zUt))))'",
     "spec-patterns\/constrained_chain\/after_q_until_r": "-f 'G(q->(p->(!rU(s&!r&!z&X((!r&!z)Ut))))U(r|G(p->(s&!z&X(!zUt)))))'",
     "spec-patterns\/constrained_chain\/before_r": "-f 'Fr->(p->(!rU(s&!r&!z&X((!r&!z)Ut))))Ur'",
     "spec-patterns\/constrained_chain\/between_q_and_r": "-f 'G((q&Fr)->(p->(!rU(s&!r&!z&X((!r&!z)Ut))))Ur)'",
     "spec-patterns\/constrained_chain\/globally": "-f 'G(p->F(s&!z&X(!zUt)))'",
     "spec-patterns\/existence\/after_q": "-f 'G(!q)|F(q&Fp))'",
     "spec-patterns\/existence\/after_q_until_r": "-f 'G(q&!r->(!rU(p&!r)))'",
     "spec-patterns\/existence\/before_r": "-f '!rW(p&!r)'",
     "spec-patterns\/existence\/between_q_and_r": "-f 'G(q&!r->(!rW(p&!r)))'",
     "spec-patterns\/existence\/globally": "-f 'F(p)'",
     "spec-patterns\/precedence\/after_q": "-f 'G!q|F(q&(!pWs))'",
     "spec-patterns\/precedence\/after_q_until_r": "-f 'G(q&!r->(!pW(s|r)))'",
     "spec-patterns\/precedence\/before_r": "-f 'Fr->(!pU(s|r))'",
     "spec-patterns\/precedence\/between_q_and_r": "-f 'G((q&!r&Fr)->(!pU(s|r)))'",
     "spec-patterns\/precedence\/globally": "-f '!pWs'",
     "spec-patterns\/response\/after_q": "-f 'G(q->G(p->Fs))'",
     "spec-patterns\/response\/after_q_until_r": "-f 'G(q&!r->((p->(!rU(s&!r)))Wr)'",
     "spec-patterns\/response\/before_r": "-f 'Fr->(p->(!rU(s&!r)))Ur'",
     "spec-patterns\/response\/between_q_and_r": "-f 'G((q&!r&Fr)->(p->(!rU(s&!r)))Ur)'",
     "spec-patterns\/response\/globally": "-f 'G(p->Fs)'",
     "spec-patterns\/universality\/after_q": "-f 'G(q->G(p))'",
     "spec-patterns\/universality\/after_q_until_r": "-f 'G(q&!r->(pWr))'",
     "spec-patterns\/universality\/before_r": "-f 'Fr->(pUr)'",
     "spec-patterns\/universality\/between_q_and_r": "-f 'G((q&!r&Fr)->(pUr))'",
     "spec-patterns\/universality\/globally": "-f 'G(p)'"
     };
     */
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

    $scope.addCommonProp = function () {
        var commonProp = $scope.commonPropSelected + " -l";
        $scope.ltl = commonProp;
        $("#commonPropsModal").modal("hide");

    };


    $scope.miningSuccess = function (data) {

        // Try to parse JSON
        try {
            var parsedData = jQuery.parseJSON(data);
        }
        catch (e) {
            showErrorModal("Sorry, there has been an error in mining the property.");
            $scope.$apply();
            return;
        }

        console.log(parsedData);
        //Emptying previous output
        $scope.bindings = [];
        $scope.properties = [];


        // Will be edited
        if (parsedData.length != 1) {
            showErrorModal("Sorry, there has been an error in mining the property.");
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
        $("#outputBtn").css({display: "block"});
        $scope.$apply();
        $("#outputBtn").click();


    };

    $scope.miningFailure = function () {
        $scope.bindings = [];
        $scope.properties = [];
        showErrorModal("Sorry, there has been an error in mining the property.");
        $scope.$apply();
        return;
    }

    $scope.mine = function () {
        //$scope.text = $scope.text.replace(/\n/g, '\\n');
        var in_str = "{\"log\" : \"" + $scope.text.replace(/\n/g, '\\n') + "\", \"args\" : \"" + $scope.ltl.replace(/\n/g, "") + "\"}";

        //var parameters = {"log" : $scope.text.replace(/\n/g, '\\n'), "args" : $scope.ltl};
        var formData = new FormData($("#uploadForm")[0]);
        console.log(formData);
        if ($scope.uploadOrText == "upload") {


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
function showErrorModal(msg) {
    $("#errorModal").modal();
}

// Called when file is selected
function fileChange() {
    if ($("#argumentsText").val().length > 0) {
        $("#mineButton").removeAttr("disabled");
    }
}

/*
 function addCommonProp() {

 var commonProp = $("#chooseCommon").val();
 if (commonProp != "") {
 var appElement = document.querySelector('[ng-app=texadaApp]');

 var $scope = angular.element(appElement).scope();
 $scope.ltl = commonProp + " -l";
 $scope.$apply();

 $("#commonPropsModal").modal("hide");
 }

 }
 */

