'use strict';


describe('Texada', function () {
    beforeEach(function () {
        browser.get('');
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

    it('should submit form if mine clicked', function () {
        $("#inputBtn").click();
        browser.sleep(500);
        $("#mineButton").click();
        browser.sleep(500);
        expect($("#output").isDisplayed()).toBeTruthy();


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
