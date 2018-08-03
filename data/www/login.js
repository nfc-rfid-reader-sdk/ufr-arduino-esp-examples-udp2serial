
function login() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
            if(this.status==403)
            {
                document.getElementById("invalid").style.display="block";
            }
            else if(this.status==200)
            {
                setCookie("token", b64EncodeUnicode(document.getElementById("username").value + ":" + document.getElementById("password").value), 1);
                window.location="/main";
            }
        }
    };
    xhttp.open("POST", "auth", false);
    xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhttp.send("username="+ document.getElementById("username").value +"&password=" + document.getElementById("password").value);
}

function hide() {
    document.getElementById("invalid").style.display="none";
}

function setCookie(cname, cvalue, exhours) {
    var d = new Date();
    d.setTime(d.getTime() + (exhours * 60 * 60 * 1000));
    var expires = "expires="+d.toUTCString();
    document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}

function b64EncodeUnicode(str) {
    return btoa(encodeURIComponent(str).replace(/%([0-9A-F]{2})/g,
        function toSolidBytes(match, p1) {
            return String.fromCharCode('0x' + p1);
        }));
}