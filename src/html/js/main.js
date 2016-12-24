var login_post = {
  type: "POST",
  url: window.location.href,
  data: {user:"dovakhiin1359", pass:"helloitsapass", save:"1"},
  success: function(msg){
    alert( "Data Saved: " + msg );
  },
  error: function(XMLHttpRequest, textStatus, errorThrown) {
    if (errorThrown == "Bad Request")
    {
        login();
    }
    if (errorThrown == "Unauthorized")
    {
        $('.wrongpass').css("display", "block");
    }
  }
}
function login(){
    $.ajax(login_post);
}
