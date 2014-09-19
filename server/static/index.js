// var url="http://kramer.nss.cs.ubc.ca:8080"
var url="http://127.0.0.1:8080"

function mineInstances() {
    var obj = document.getElementById("log");
    var log = obj.value;
    var dst = url + "/mine/" + name;
    $.get(dst, function( data ) {
        $("#result1").html( data );
        obj = JSON && JSON.parse(data) || $.parseJSON(data);
        // Help the humans by propogating the returned key from the
        // JSON returned by initSession to the other text-boxes for
        // key on the page.
        $("#getwaldos_key").val( obj.Key );
        $("#getwaldobyname_key").val( obj.Key );
        $("#postlocation_key").val( obj.Key );
        $("#sendmsg_key").val( obj.Key );
        $("#getmsgs_key").val( obj.Key );
    });
}

