'use strict';

(function () {
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

    texada.controller("TexadaHomeCtrl", ["$scope", "$http", function ($scope, $http) {
        $scope.ltl = "-f 'G(x->XFy)' -l";    // The LTL property to be mined
        $scope.currentPropertyType = "always-followed-by"; // LTL property name to display
        // The log/data to mine
        $scope.text = "StackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--\nStackAr(int)\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\npush(java.lang.Object)\nisFull()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\nmakeEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\ntopAndPop()\nisEmpty()\nisFull()\nisEmpty()\ntop()\nisEmpty()\n--";
        $scope.defaultErrorMessage = "Sorry, an unknown error occurred.";
        $scope.uploadOrText = "text";      // File upload or textbox (textbox used as default)
        $scope.commonPropSelected = "";    // Selected common prop

        // Common property types to be used
        $scope.commonProps = {
            "immediately-followed-by\/followedby": "G(x -> X y)",
            "perracotta\/alternating": "(!y W x) & G((x -> X(!x U y))&(y -> X(!y W x)))",
            "perracotta\/causefirst": "(!y W x) & G(x -> XFy)",
            "perracotta\/effectfirst": "G((x -> X(!x U y)) & (y -> X(!y W x)))",
            "perracotta\/multicause": "(!y W x) & G((x -> XFy) & (y -> X(!y W x)))",
            "perracotta\/multieffect": "(!y W x) & G(x -> X(!x U y))",
            "perracotta\/onecause": "G(x -> X(!x U y))",
            "perracotta\/oneeffect": "G((x -> XFy) & (y -> X(!y W x)))",
            "perracotta\/response": "G(x->XFy)",
            "synoptic\/always-followed-by": "G(x -> XF y)",
            "synoptic\/always-precedes": "Fy -> (!y U x)",
            "synoptic\/never-followed-by": "G(x -> G !y)",
            "spec-patterns\/absence\/after_q": "G(q -> G(!p))",
            "spec-patterns\/absence\/after_q_until_r": "G(q & !r -> (!p W r))",
            "spec-patterns\/absence\/before_r": "Fr -> (!p U r)",
            "spec-patterns\/absence\/between_q_and_r": "G((q & !r & Fr) -> (!p U r))",
            "spec-patterns\/absence\/globally": "G(!p)",
            "spec-patterns\/bounded_existence\/after_q": "Fq -> (!q U (q & (!p W (p W (!p W (p W G!p))))))",
            "spec-patterns\/bounded_existence\/after_q_until_r": "G(q -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p W r) | Gp)))))))))",
            "spec-patterns\/bounded_existence\/before_r": "Fr -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p U r)))))))))",
            "spec-patterns\/bounded_existence\/between_q_and_r": "G((q & Fr) -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p U r))))))))))",
            "spec-patterns\/bounded_existence\/globally": "(!p W (p W (!p W (p W G!p))))",
            "spec-patterns\/constrained_chain\/after_q": "G (q -> G (p -> (s & !z & X(!z U t))))",
            "spec-patterns\/constrained_chain\/after_q_until_r": "G (q -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U (r | G (p -> (s & !z & X(!z U t)))))",
            "spec-patterns\/constrained_chain\/before_r": "Fr -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r",
            "spec-patterns\/constrained_chain\/between_q_and_r": "G ((q & Fr) -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r)",
            "spec-patterns\/constrained_chain\/globally": "G (p -> F(s & !z & X(!z U t)))",
            "spec-patterns\/existence\/after_q": "G(!q) | F(q & Fp))",
            "spec-patterns\/existence\/after_q_until_r": "G(q & !r -> (!r U (p & !r)))",
            "spec-patterns\/existence\/before_r": "!r W (p & !r)",
            "spec-patterns\/existence\/between_q_and_r": "G(q & !r -> (!r W (p & !r)))",
            "spec-patterns\/existence\/globally": "F(p)",
            "spec-patterns\/precedence\/after_q": "G!q | F(q & (!p W s))",
            "spec-patterns\/precedence\/after_q_until_r": "G(q & !r -> (!p W (s | r)))",
            "spec-patterns\/precedence\/before_r": "Fr -> (!p U (s | r))",
            "spec-patterns\/precedence\/between_q_and_r": "G((q & !r & Fr) -> (!p U (s | r)))",
            "spec-patterns\/precedence\/globally": "!p W s",
            "spec-patterns\/response\/after_q": "G(q -> G(p -> Fs))",
            "spec-patterns\/response\/after_q_until_r": "G(q & !r -> ((p -> (!r U (s & !r))) W r)",
            "spec-patterns\/response\/before_r": "Fr -> (p -> (!r U (s & !r))) U r",
            "spec-patterns\/response\/between_q_and_r": "G((q & !r & Fr) -> (p -> (!r U (s & !r))) U r)",
            "spec-patterns\/response\/globally": "G(p -> Fs)",
            "spec-patterns\/universality\/after_q": "G(q -> G(p))",
            "spec-patterns\/universality\/after_q_until_r": "G(q & !r -> (p W r))",
            "spec-patterns\/universality\/before_r": "Fr -> (p U r)",
            "spec-patterns\/universality\/between_q_and_r": "G((q & !r & Fr) -> (p U r))",
            "spec-patterns\/universality\/globally": "G(p)"
        };


        $scope.bindings = [];      // The results fetched: the bindings found
        $scope.properties = [];    // The results fetched: the properties mined


        // Respond to changes in the args text box.
        // If args are empty, then disable mine button
        $scope.$watch("ltl", function (value, old) {

            // Loop through the common property types object key-val
            // and find the name of the selected property.
            $scope.currentPropertyType = "--";
            for (var key in $scope.commonProps) {
                if ($scope.getPropTexadaCmd($scope.commonProps[key]) == value) {
                    // Set property name displayed on page.
                    $scope.currentPropertyType = key;
                }
            }

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

        // This converts a property string in LTL format into a
        // version that we can pass to texada on the command line
        // (adding the necessary texada command line flags).
        $scope.getPropTexadaCmd = function (prop) {
            return "-f '" + prop + "' -l";
        };


        // Add common property type
        $scope.addCommonProp = function () {
            var commonProp = $scope.getPropTexadaCmd($scope.commonPropSelected);
            $scope.ltl = commonProp;
            $("#commonPropsModal").modal("hide");
        };

        // Server returned a 200 OK response
        $scope.miningSuccess = function (data) {
            data = data.data;

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
            $("#outputBtn").tab("show");


        };

        // Server returned an error
        $scope.miningFailure = function (data) {
            $scope.bindings = [];
            $scope.properties = [];
            console.log(data);
            if (data.status != 200) {
                showErrorModal($scope.defaultErrorMessage);
                return;
            }
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
                /*$.ajax({
                 "method": "POST",
                 "url": "/texada/uploadMine/",
                 "data": formData,
                 cache: false,
                 contentType: false,
                 processData: false
                 }).done($scope.miningSuccess).fail($scope.miningFailure);*/
                return $http({
                    "method": "POST",
                    "url": "/texada/uploadMine/",
                    "data": formData,
                    cache: false,
                    headers: {'Content-Type': undefined},
                    transformRequest: undefined,
                    transformResponse: undefined
                }).then($scope.miningSuccess, $scope.miningFailure);
            }
            else {
                /*$.ajax({
                 "method": "POST",
                 "url": "/texada/mine/",
                 "data": in_str
                 }).done($scope.miningSuccess).fail($scope.miningFailure);*/
                return $http({
                    "method": "POST",
                    "url": "/texada/mine/",
                    "data": in_str,
                    transformResponse: undefined
                }).then($scope.miningSuccess, $scope.miningFailure);
            }
        }


    }]);

    // Directives

    // Custom directive for the commonProps modal
    texada.directive("commonProps", function () {
        return {
            restrict: 'E',
            templateUrl: 'templates/commonProps.html',
            scope: false,
            controller: ["$timeout", "$scope", function ($timeout, $scope) {
                $timeout(function () {
                    $("#chooseCommon").chosen({width: "100%"});
                });
            }]
        }
    });

    // Custom directive for the commonProps modal
    texada.directive("errorMessage", function () {
        return {
            restrict: 'E',
            templateUrl: 'templates/errorMessage.html',
            scope: false
        }
    });

    // Custom directive for the splash screen
    texada.directive("splashScreen", function () {
        return {
            restrict: 'A',
            templateUrl: 'templates/splashScreen.html',
            scope: false
        }
    });

    // Custom directive for the input tab
    texada.directive("inputTab", function () {
        return {
            restrict: 'A',
            templateUrl: 'templates/inputTab.html',
            scope: false
        }
    });

    // Custom directive for the output tab
    texada.directive("outputTab", function () {
        return {
            restrict: 'A',
            templateUrl: 'templates/outputTab.html',
            scope: false
        }
    });


})();


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
