describe('TexadaApp', function () {

    // You need to load modules that you want to test,
    // it loads only the "ng" module by default.
    beforeEach(module('texadaApp'));

    describe('Texada Home Controller', function () {
        var $controller;
        var $scope;

        beforeEach(inject(function ($rootScope, _$controller_, $httpBackend) {
            $scope = $rootScope.$new();
            $controller = _$controller_('TexadaHomeCtrl', {
                '$scope': $scope
            });
            spyOn($scope, 'miningSuccess').and.callThrough();
            $httpBackend.whenPOST("/texada/uploadMine/").passThrough();

        }));


        it("should have default LTL value", function () {
            expect($scope.ltl).not.toBeNull();
            expect($scope.ltl).not.toBe("");
        });

        it("should have default log", function () {
            expect($scope.text).not.toBeNull();
            expect($scope.text).not.toBe("");
        });



    });


});