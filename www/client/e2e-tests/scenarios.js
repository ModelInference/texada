'use strict';


describe('Texada', function () {
    beforeEach(function () {
        browser.ignoreSynchronization = true;
        browser.get('');
        browser.waitForAngular();
    });

    it('should have splash screen video loaded', function () {
        expect($(".youtubevideo").getAttribute("src")).toBe("https://www.youtube.com/embed/sNeGZG1LN9U");
    });

    it('should have default args', function () {
        expect($("#argumentsText").getAttribute("value")).toBe("-f 'G(x->XFy)' -l");
    });

    it('should have default log', function () {
        expect($("#inputText").getAttribute("value")).not.toBeNull();
        expect($("#inputText").getAttribute("value")).not.toBe("");
    });

    it('should show one property type with appropriate bindings given the default LTL and log', function () {
        $("#inputBtn").click();
        browser.sleep(500);
        $("#mineButton").click();
        browser.sleep(500);
        expect($("#output").isDisplayed()).toBeTruthy();

        var expectedProps = ["x", "y"];

        element.all(by.binding('b.prop')).then(function(items){
            expect(items.length).toBe(2);
            for(var i = 0; i < items.length; i++){
                expect(items[i].getText()).toBe(expectedProps[i]);
            }
        });

        var expectedVars = ["b", "c"];

        element.all(by.binding('b.var')).then(function(items){
            expect(items.length).toBe(2);
            for(var i = 0; i < items.length; i++){
                expect(items[i].getText()).toBe(expectedVars[i]);
            }
        });

    });

    it('should show no property type with appropriate bindings given the immediately followed by LTL and default log', function () {
        var ltl = "-f 'G(x -> X y)' -l";
        $("#inputBtn").click();
        browser.sleep(500);

        $("#argumentsText").clear().then(function () {
            $("#argumentsText").click().then(function () {
                $("#argumentsText").sendKeys(ltl).then(function () {
                    $("#argumentsText").getAttribute("value").then(function (ltlVal) {
                        expect(ltlVal).toEqual(ltl);

                        $("#inputBtn").click();
                        browser.sleep(500);
                        $("#mineButton").click();
                        browser.sleep(500);

                        expect($("#output").isDisplayed()).toBeTruthy();


                        element.all(by.binding('b.prop')).then(function (items) {
                            expect(items.length).toBe(0);
                        });


                        element.all(by.binding('b.var')).then(function (items) {
                            expect(items.length).toBe(0);
                        });

                    });
                });
            });

        });


    });

    it('should not enable mine button if log is empty', function () {
        $("#inputBtn").click();
        browser.sleep(500);
        element(by.model("text")).clear();
        expect($("#mineButton").getAttribute("disabled")).not.toBeNull();
    });

    it('should not enable mine button if no file uploaded but upload is selected', function () {
        $("#inputBtn").click();
        browser.sleep(500);
        $("#uploadOption").click();
        expect($("#mineButton").getAttribute("disabled")).not.toBeNull();
    });

    it('should not enable mine button if args is empty', function () {
        $("#inputBtn").click();
        browser.sleep(500);
        element(by.model("ltl")).clear();
        expect($("#mineButton").getAttribute("disabled")).not.toBeNull();
    });

});
