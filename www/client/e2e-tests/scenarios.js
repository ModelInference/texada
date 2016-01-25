'use strict';

/* https://github.com/angular/protractor/blob/master/docs/toc.md */

describe('Texada', function () {


    it('should automatically redirect to /view1 when location hash/fragment is empty', function () {
        browser.get('');
        expect(browser.getTitle()).toBe("Texada");
    });



});
