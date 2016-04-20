$(document).ready(function() {
    $("#h").click( function() {
        $("#home").show()
        $("#form1").hide()
        $("#allout").hide()
    });
    
    $("#i").click( function() {
        $("#form1").show()
        $("#home").hide()
        $("#allout").hide()
    });    
    
    $("#r").click( function() {
        $("#allout").show()
        $("#home").hide()
        $("#form1").hide()
    });    
    
})