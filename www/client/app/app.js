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

    $scope.commonPropSelected = "";

    $scope.commonProps = {
        "immediately-followed-by": "-f 'G(x->XFy)'",
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

    $scope.bindings = [];      // The results fetched: the bindings found
    $scope.properties = [];    // The results fetched: the properties mined

    $scope.addCommonProp = function () {
        var commonProp = $("#chooseCommon").val() + " -l";
        $scope.ltl = commonProp;
        $("#commonPropsModal").modal("hide");
    };

    $scope.mine = function () {
        //$scope.text = $scope.text.replace(/\n/g, '\\n');

        var in_str = "{\"log\" : \"" + $scope.text.replace(/\n/g, '\\n') + "\", \"args\" : \"" + $scope.ltl.replace(/\n/g, "") + "\"}";

        //var parameters = {"log" : $scope.text.replace(/\n/g, '\\n'), "args" : $scope.ltl};

        $.ajax({
            "method": "POST",
            "url": "/texada/mine/",
            "data": in_str
        }).done(function (data) {

            var parsedData = jQuery.parseJSON(data);
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

            $scope.$apply();
            $("#outputBtn").click();


        }).fail(function () {
            $scope.bindings = [];
            $scope.properties = [];
            showErrorModal("Sorry, there has been an error in mining the property.");
            $scope.$apply();
            return;
        });
    }
});
// Tests


// Services


// Document ready
$(document).ready(function () {
    // Activate tabs
    //$("#myTab li:eq(1) a").tab('show');
    $("#chooseCommon").chosen({width: "100%"});
});

// Functions
function showErrorModal(msg) {
    $("#errorModal").modal();
}



