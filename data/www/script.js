var img1 = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAQAAADZc7J/AAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JQAAgIMAAPn/AACA6QAAdTAAAOpgAAA6mAAAF2+SX8VGAAAAAmJLR0QA/4ePzL8AAAAJcEhZcwAADdcAAA3XAUIom3gAAAAHdElNRQfiBRMGCTYH4TVKAAADHElEQVRIx93UX2jVdRjH8dc553fO2cxNmpVzLeaCtpoWgUi1stkfIuzPRd0mZhYkIkRB3gRFaApBJHaVJeJFQoWpRRddlGQklaEuNQ+12DC1XG7nnO1sO39/Xewc3XKVt/Xcfp/n/Xmez/P9fvnPR+Ty0lIQSIqLCuWNCzsvD5Ai0KzLIjdo1iih4qwtDtKJ4F9UGyzxiLtdr1F0yuE53yr7J0CKOkutdp+51T4LxuUVxYw5NFn+N4AULLDWCvMQ+sMxh5zwq4y8mAl9kwPM6EGKiG6v6hET6rfPXr3Syp2Xs4UUUQ/b7CYMe982vYogabbZEsoGjdQ6CGZQf9Qb2nHca/YYQ70uSy22QJOksv1eMjiDBym4yybtOOBF3wgldVvpXi1iFxLneHdGANq84kZ8ZZ2jaLXWSvNBXtqQCVEH9NUKgmn6CWv04EfrHcUimzwoEDrlc1846Zy8iLSxSwAp6LZCTNomB7HQVsuQ8YFtek3UjJsaU0eYZZUWfGi3UIuNlmHABrvkEHdVar4GWSeNd04HpGCxB9Bvm5y4NR7CgOftVZZwm8d1a1UnZ4O3U+Ff1xi1XDM+dQS3e1Iga6M9KuZa52nXVjOvdKugejemAK7Wg2H7FCQ9oRW77VIx1warJIXO+9l5Q3bWyqcCOnXgpCPocD/OeMeowLNWSRr1kR2OGVOSv2hncMGBLnNw2Hnc4Tp86TAWe0ZSzuvelL10D7UOojoESo4riVsirmi/MTGPacMeW77PrktM3FzXETmX/C6dDavPqAZImI8Jp3CFdmSdQJM7MWTn6cxzwcjq0vqwWa70XuPLs4dqypMR14iCNOo1IeN3XKMV/Y7tMNpVeqHSFibDptJT+eU5bdMAEQHKiohLoKCAWeqRlutTaQ+bJ5PDWZWu3kh+GiBURkwcJUVERTEig4xSTPS3SLqqVogO3BLWTQMUDKKkhGFfG3HYMH6x2XZvyY2rPxrbHskQycc+Dj6p019tvbbGHmv8YKss5lnoJ6eqEoGisFODaEP+nsqiyOn4Z6NnW5y5CKj+RHWKSjP9e//z+BPZPhDNMXY8CQAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxOC0wNS0xOVQwNjowOTo1NC0wNDowMB4Jm8IAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTgtMDUtMTlUMDY6MDk6NTQtMDQ6MDBvVCN+AAAAAElFTkSuQmCC';
var img2 = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAQAAADZc7J/AAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JQAAgIMAAPn/AACA6QAAdTAAAOpgAAA6mAAAF2+SX8VGAAAAAmJLR0QA/4ePzL8AAAAJcEhZcwAADdcAAA3XAUIom3gAAAAHdElNRQfiBRMGCg4Ezt4XAAADZ0lEQVRIx93UX2jVdRjH8dfv/N2cc62xNJ2Jm7lKNMU/QSWZYA4KIYKgP1QolV50VYkRIRhhZl5IXaSkJV2U9Acpg0gq0i5cDtLU4miWf2pOXds829nZ2c75/bpw2marvK3n6gvf53k/n+f58P3yn4/gytIykJCWEhfqkxc1XhkgQ8JYN5puqvHGSAm12uBbGpH4l64V5rjHAlNUig+5PKFF6J8AGVJutdRitYM6++XlFcVlL5b/DSADdVZ4zHhEOhzS4pBTuhTE9Tp+YYARd5CBedZYKCly0k477Nep1HglLmQI3OUVM5D1oTfs1w/SKo2WVnJW9qKCxAjdF9moEUet9b4elLnBfHPVq5FW8rXnnBthBxmYbZ1G7POs3SIpt3jEIhOG5FbaPCIA11ptJlo8pRnjLLdMHSjo1CEv8I1jFwsSw/rHLdOEn63SjKleskRKpNVXvnDYWQWcl/8LYFD+Ukk91vsS19tosUCPj2zyXZiP+asPQ0dIe9Rk7PSeSK01Fgu0Wmub7gVKyXztqPHhmKAndTjMdY8AmOZutNmsS9xS9wqcttL2rcUtyd65A/eHt0V1UXnQV1xXtzEIs0MBGWgyEbvsxUyPS+u13vani3uq8yuKy8OJF3KjqtjN2WRYuFxBtTvF5HwsL+FBDfjU228V91yVW116MioTxTqDo9qDrsTW3wu1ei4D1LsJP9mHyZrQ7s1fO7fF88tKT0RlQT7+SXJL8kCQixVHFUrahu4gA42uxkFtmKMeezWv1zW9uDwqDwrx1ytePt8xQzM6hywuduk0RZnIDwpiZisT2n3gfFtQXBI2EPu8/NWBjtrE97PSD5U1jalm3mUuJExAv5Mo14CcQwXHxoS3C4Jc/J3us5Nipx8uvhBN0FfcUbHqYNtwBQlV6NeJtBr0OHNEqSa6jqA1sT/tXEPxmbA+SkdVpQf678urGQaISSAygLg0BvRlReXKkQ2yoeLEqG7QyFQ0bVJ8YBggUkIgiZJ+xMWrBDlZgu7YQEzsTDD4AoNS8MuJUnIYYED7IIicFr1+1D5d2W/JDYl3E69Vd6VUHIlvCtoJ+mOfJT9Iev4C7JKNd1jpuBe1YZzZjsmIFkrGcqlrCj3RCaPFywvzw1nBucSu3KmxzvwJGPzIKg3IM9LP97+OPwCcF0BtAfyDWwAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxOC0wNS0xOVQwNjoxMDoxNC0wNDowMKfYYTIAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTgtMDUtMTlUMDY6MTA6MTQtMDQ6MDDWhdmOAAAAAElFTkSuQmCC';
var img3 = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAQAAADZc7J/AAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JQAAgIMAAPn/AACA6QAAdTAAAOpgAAA6mAAAF2+SX8VGAAAAAmJLR0QA/4ePzL8AAAAJcEhZcwAADdcAAA3XAUIom3gAAAAHdElNRQfiBRMGCh2AcJ/JAAADs0lEQVRIx+WUW2hcRRjHfzNnztk9zWWTNLVJ7NKkkcSS1ppqW0hLa6NWxQvog0UESxURKj4IVYpIhT4YUCko9aEQ8YJaSn1pQbRqUBCURmuIVMqUNGlNTDYJm8vu5iS75+wZH3Jr6rbmVZx5Geby+/7/+WY++M83sbxtGsAhQgQLg8cUpnF5AA2KaprYRAPVxHAI6aeN89AIqH+JWsw2HmUXdZQgF5a20s352aG6yfEI23mO+1k1N5XFw8NHkqRzfp+6Yew4B9hHNWBI8ju/cIF+xskhmWJg1kDBO9Ag2MoRWlEYrnCG03TLCRM2LCcLGgR7eJuNwCSnOE43/mPERVfUj4WlJkqgBicn1nK1EEAD7OE9GoFLvMkX/tQGKlbMbAh2hlvMOlNBhFB2RF7Nj84UEq/Rm/Vv2mijz+mdWjxOZcTdrT6RAyLAzHfxV7TZucElVnGYZuBXXkp3PkC2Jvtifn9YDSBmxBhJ4YH8wenNk7seoMHiWR4C+jiU7NxF5PaZtuBhbIwclB2yQ/1hDVtZg0yZbKqg/Lv1ZW10Rh/okY0UNahvCDEirT52W1a7sKVAFq614PAM64AvOXE0HLwleyS4DyGHVFvJR8m0YqV9scpdY2IiY3eH6UwBBXfqXm10Qt/7LTWWfUhkMTIR3RdXZax03B3qmOwSIyIlR+2DdbK0gIFXdKCN/lwX7cC9S/ZghGcfXGMXEyt3Xpf9i1lQH6xyiv9hIUYrFh5neqe08p8K60GejX445ttlU2/kXzBRjEwKLUYZs9tHc6vJXAeopQno49zXTK0NHwQxrtrTybgc3p9/3kTFtHXabne6paeCslxu7h1ea+BJ7WmjT1xym3D2Cg9jnS0tj+LeITVGzKi3YuXQepMs1BMFLrZPZ0S42bgY+WNq3OA8Ym4D+V30HX+80v55U2Qj485P6ZE6+gAWioRFDYIcf7Yw7pp6EJ64oIiVhtuNFJ71WWakRqaezp7KHc99On3MjQ/NHZQLSmKAz0SavGMqAE8mJPkysxbEsOqKMFgbvBzWGtsUBU/4e2coXwIQ2IDBz2FZIgIEZCEfNS6QEWlDeKuJz+s166vsYAnA4M9iHMzsP1HYID0xCWSMLxAJMa87lD1Dvr0EEDAGCGQ1tie6RFZelqMSN6GOqpP2u2VJh5Je9b4YEqHIWl/ZJx12LxYUDXAPh0nwWnhlPUXVQYvVW9mdCVcQkSmnMueFVylBRaa3hc1yTH2fGagisQgADZJyfFLzxfJ/1P4GHvGKGujvQp4AAAAldEVYdGRhdGU6Y3JlYXRlADIwMTgtMDUtMTlUMDY6MTA6MjktMDQ6MDBIgAcRAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDE4LTA1LTE5VDA2OjEwOjI5LTA0OjAwOd2/rQAAAABJRU5ErkJggg==';
var img4 = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAQAAADZc7J/AAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JQAAgIMAAPn/AACA6QAAdTAAAOpgAAA6mAAAF2+SX8VGAAAAAmJLR0QA/4ePzL8AAAAJcEhZcwAADdcAAA3XAUIom3gAAAAHdElNRQfiBRMGCyDBA+KZAAACpUlEQVRIx93UT2zURRwF8M/+7yK1W6SkgI3ECiZWjBYRJMZGJOrByMVAjJpw8IIXa2LEeDKexAQjid6qwcQ2MWliuKDEmzFR/oSoNIpgJbZQt6Fmi3a3212648HftiC7gEd9c/hlZr7vzZt5vxn+84j9i9qUjLSkYFZRuHGBpE497rHOSm3Sas7Z63h98tpY6n5P6dOtVfyy8R9vRCBjixds0xH5nDOrpCqh4OiivWbostsuKxFMGXHMiHEFFXElY9fb9yafqwpqztrvEbdcsYHrJvO47wTBtAEbpaLxFit0u8udbr62wGNOCYKf7HITyNqg36CvnfarswYsb07vdUIQHNUnhrSHfWhMVVhoE3qb0TsdFATHbIr6bzoX0Wad970jjtgr15ie8LqKYNSjYJ1hFUHNuI88r1eXFTq0NFt/o1HBjN3iWOszNcEfDniwOWkRae8Jgk/k0GFITXDei1pByioP2GazpY0F7vOLIG8rEl5VFkx4VhIpW7zruLyLJvU3/iv2mBcMWoJeZwRFL0si5zVjl6UwIH01PeewoGgHkt4WBMOWoc07SoKaC77yqQ9sbrT+vcYFJ61Bt5OCKU8g7iVFQcmgrZZbItP4BHYqCYa0YIeS4JAc1jslKHtLeyPa4m28QxY/KIvZICv40jSetBaH7VOQtF6Pgm/8/k+BpFWoGENWN4pG0OohcUUfuyDuGW+41ZyD9pj4m1iPI6ENVdNIW4aiSbS7Db/5Fmu84nZprXZ6uu4gvvBNoaaKhAyqymiRxZ9msFpXVJ3SU/deFwguLcjMqyApiZKLmFHBpHxUPe9nl64UqEbHEkPJCWVnTCFvnyH7FTDqfXk1cw4ZvjqPPl84EJnstN3d0VMal1l4/DP69HvO6kaBxrQ3uyT/c/wFmALh470ZkEAAAAAldEVYdGRhdGU6Y3JlYXRlADIwMTgtMDUtMTlUMDY6MTE6MzItMDQ6MDBp7zhLAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDE4LTA1LTE5VDA2OjExOjMyLTA0OjAwGLKA9wAAAABJRU5ErkJggg==';
var connssid = 0;

$(document).ready(function () {
    if (getCookie("token") == "") {
        window.location = "/";
    }
    else {
        info();
    }
});

function getCookie(cname) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

function info() {
    $.ajax({
        url: 'info',
        type: 'GET',
        contentType: 'json',
        beforeSend: function (xhr) {
            xhr.setRequestHeader("Authorization", "Basic " + getCookie("token"));
        },
        success: function (resp) {
            if (resp.status == "connected") {
                connssid = resp.ssid;
                scan(1);
            }
            else {
                scan(1);
            }
        }
    });
}

function scan(mod) {
    $.ajax({
        url: 'scan',
        type: 'GET',
        contentType: 'json',
        beforeSend: function (xhr) {
            $("body").css("overflow-y", "hidden");
            if (mod) {
                $("#loader-wrapper").fadeIn(400);
            }

            xhr.setRequestHeader("Authorization", "Basic " + getCookie("token"));
        },
        success: function (resp) {
            $("#net").empty();
            $("#connected").empty();
            if (connssid) {
                $('#connected').append("<h3>CONNECTED TO: </h3>");

            }
            //var resp = JSON.parse(json);

            document.getElementById("net").innerHTML = "<h3 onclick='scan(1)' style='cursor: pointer'>AVAILABLE NETWORKS (" + resp.result.length + "): &#x21bb;</h3>";
            resp.result.forEach(function (e) {
                var parent = document.getElementById("net");


                if (e.ssid == connssid) {
                    parent = document.getElementById("connected")
                }

                var element = document.createElement("div");
                element.setAttribute("class", "element");
                var icon = document.createElement("div");
                icon.setAttribute("class", "icon");
                var img = document.createElement("img");
                var wifi = document.createElement("div");
                wifi.setAttribute("class", "wifi");
                var ssid = document.createElement("div");
                ssid.setAttribute("class", "ssid");
                var channel = document.createElement("div");
                channel.setAttribute("class", "channel");
                var bar = document.createElement("div");
                bar.setAttribute("class", "bar");
                var innerBar = document.createElement("div");
                innerBar.setAttribute("class", "innerbar");
                var connect = document.createElement("button");
                if (e.ssid == connssid) {
                    connect.innerHTML = "Disconnect";
                    connect.setAttribute("class", "connect");
                    connect.setAttribute("onclick", "disconnect()");
                }
                else {
                    connect.innerHTML = "Connect";
                    connect.setAttribute("class", "connect");
                    connect.setAttribute("onclick", "onConnect('" + e.ssid + "', '" + e.encryption + "')");
                }
                var dbm = parseInt(e.signal);
                var perc = 100 * (1 - (30 + dbm) / (30 - 100));
                element.onclick = function () {
                    var x = document.getElementsByClassName('connect');
                    for (var k = 0; k < x.length; k++) {
                        if (x[k] == connect) {
                            x[k].style.display = 'block';
                        } else {
                            x[k].style.display = 'none';

                        }


                    }
                };
                if (e.ssid == connssid) {
                    element.setAttribute("ondblclick", "disconnect()");
                }
                else {
                    element.setAttribute("ondblclick", "onConnect('" + e.ssid + "', '" + e.encryption + "')");
                }

                img.setAttribute("width", "32");
                img.setAttribute("height", "32");
                img.src = setimg(Math.abs(dbm));
                ssid.innerHTML = e.ssid + " | " + e.encryption;
                channel.innerHTML = "CH " + e.channel;
                innerBar.style.width = perc + "%";
                innerBar.style.backgroundColor = colors(Math.abs(dbm));
                innerBar.innerHTML = dbm + " dBm";


                element.appendChild(icon);
                element.appendChild(wifi);
                element.appendChild(connect);
                icon.appendChild(img);
                wifi.appendChild(ssid);
                wifi.appendChild(channel);
                wifi.appendChild(bar);
                bar.appendChild(innerBar);

                parent.appendChild(element);

            });

            $("#top").show();
            $("#connected").show();

            $("#net").show();
            $("#loader-wrapper").fadeOut(400);
            $("body").css("overflow-y", "auto");

        }
    });
}

function disconnect() {
    dialog.confirm({
        title: "Disconnect from " + connssid + "?",
        message: "",
        cancel: "No",
        button: "Yes",
        required: true,
        callback: function (value) {
            if (value) {
                $.ajax({
                    url: 'disconnect',
                    type: 'GET',
                    beforeSend: function (xhr) {

                        xhr.setRequestHeader("Authorization", "Basic " + getCookie("token"));
                    },
                    success: function (resp) {
                        if (resp == "1") {
                            connssid = 0;
                            scan(1);
                        }
                        else {
                            dialog.alert({
                                title: "Cannot disconnect from " + connssid,
                                message: ""
                            });
                        }
                    }
                });
            }
        }
    });

}

function onConnect(ssid, encryption) {
    if (encryption != "Open") {
        dialog.prompt({
            title: "Connect to " + ssid,
            message: "",
            button: "Connect",
            required: false,
            input: {
                type: "password",
                placeholder: ssid + " password..."
            },
            callback: function (value) {
                if ($.trim(value) === "") {
                    return false;
                }
                else {
                    $.ajax({
                        url: 'changesta',
                        type: 'POST',
                        data: {ssid: ssid, pass: value},

                        beforeSend: function (xhr) {
                            $("body").css("overflow-y", "hidden");
                            $("#loader-wrapper").fadeIn(400);
                            xhr.setRequestHeader("Authorization", "Basic " + getCookie("token"));
                        },
                        success: function (resp) {
                            if (resp == "3") {
                                connssid = ssid;
                                scan(0);
                            }
                            else {
                                dialog.alert({
                                    title: "Cannot connect to" + nssid,
                                    message: ""
                                });
                                scan(1);


                            }
                        }
                    });
                }
            }
        });
    }
    else {

    }

}

function setimg(dbm) {
    if (dbm <= 50) {
        return img4;
    }
    else if (dbm <= 60) {
        return img3;
    }
    else if (dbm <= 70) {
        return img2;
    }
    else if (dbm > 70) {
        return img1;
    }
}

function colors(dbm) {
    if (dbm <= 50) {
        return '#2ecc71';
    }
    else if (dbm <= 60) {
        return '#f1c40f';
    }
    else if (dbm <= 70) {
        return '#e67e22';
    }
    else if (dbm > 70) {
        return '#c0392b';
    }
}
