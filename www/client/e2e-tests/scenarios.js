'use strict';


describe('Texada', function () {
    beforeEach(function () {
        browser.get('');
    });

    it('should have title "Texada"', function () {
        expect(browser.getTitle()).toBe("Texada");
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
        $("#inputBtn").click().then(function () {
            $("#mineButton").click().then(function () {
                expect($("#outputTab").isDisplayed()).toBeTruthy();
            });
        });


    });



});
