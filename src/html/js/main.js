var encrypt = new JSEncrypt();
encrypt.setPublicKey("-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDxrHo1+SMguB/UOytsPhjlDx4B\n\
Ix5611YfxyVqPS46szesaQVBwNiIaiMU2IceGMxvF8flovrZ+QbTY9Sd365aNMn7\n\
VpMkGfPa57Ji6COmA/gCBFy1HJ6mJs//xgJRpmDSxzm1LBwmO3w1CbZQh/LFDg4c\n\
+xREZwG+S0JNwDnUGwIDAQAB\n\
-----END PUBLIC KEY-----");

String.prototype.format = function() {
    var formatted = this;
    for (var i = 0; i < arguments.length; i++) {
        var regexp = new RegExp('\\{'+i+'\\}', 'gi');
        formatted = formatted.replace(regexp, arguments[i]);
    }
    return formatted;
};

function enc_pass(user, pass)
{
    if (user == "" || pass == "")
    {
        return encrypt.encrypt("user={0}\\pass={1}\\save=1".format ("placeholder", "passholder"));
    }
    return encrypt.encrypt("user={0}\\pass={1}\\save=1".format (user, pass));
}

function login(){
    $.ajax({
    type: "POST",
    url: window.location.href,
    data: enc_pass($("#email").val(), $("#passwd").val()),
    success: function(msg){
        
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
        if (errorThrown == "Bad Request")
        {
            //login();
        }
        if (errorThrown == "Unauthorized")
        {
            $('.wrongpass').css("display", "block");
        }
    }
});
}
