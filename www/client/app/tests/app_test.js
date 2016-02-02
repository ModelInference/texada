describe('TexadaApp', function () {

    // You need to load modules that you want to test,
    // it loads only the "ng" module by default.
    beforeEach(module('texadaApp'));

    describe('Texada Home Controller', function () {
        var $controller;
        var $scope;

        beforeEach(inject(function ($rootScope, _$controller_) {
            $scope = $rootScope.$new();
            $controller = _$controller_('TexadaHomeCtrl', {
                '$scope': $scope
            });
            spyOn($scope, 'miningSuccess').and.callThrough();

        }));

        it("should have default LTL value", function () {
            expect($scope.ltl).not.toBeNull();
            expect($scope.ltl).not.toBe("");
        });

        it("should have default log", function () {
            expect($scope.text).not.toBeNull();
            expect($scope.text).not.toBe("");
        });

        it("should populate JSON on mining", function () {
            $scope.ltl = "-f 'G(x->XFy)' -l";    // The LTL property to be mined
            $scope.text = "a\nb\nc\n--\nb\nb\nc\na\n--\nc\na\nb\nc\n--";   // The log/data to mine
            $scope.mine().then(function () {
                expect($scope.miningSuccess).toHaveBeenCalled(); // should be successful ideally
                expect($scope.properties).toContain("G(x->XFy)"); // should have at least one instance of given LTL prop type
            });


        });
    });


});