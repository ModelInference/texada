 $( document ).ready(function() {

   $("#t").click( function () {
       $("#home").addClass("hide")
       $("#tryit").removeClass("hide")
       $("#allout").addClass("hide")
   });

   $("#h").click( function () {
       $("#home").removeClass("hide")
       $("#tryit").addClass("hide")
       $("#allout").addClass("hide")
   });

   $("#r").click( function () {
       $("#home").addClass("hide")
       $("#tryit").addClass("hide")
       $("#allout").removeClass("hide")
   });
})
