const char MAIN_PAGE[] = R"=====(
<!doctype html>
<html lang="en">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"
        integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

    <style>
        hr {
            display: block;
            margin-end: auto;
            margin-start: auto;
            border: 1px inset;
            overflow: hidden;
            margin-before: 0.5em;
            margin-after: 0.5em;
        }

        body {
            padding-bottom: 80px;
            font-family: Calibri, Helvetica, sans-serif;
        }

        .slidecontainer {
            width: Calc(100% - 100px);
            margin: 0 auto;
        }

        .slider {
            width: Calc(100% - 196px);
            /*196px means both sides (left 98px and right 98px)*/
            margin: 0 auto;
            -webkit-appearance: none;
            height: 20px;
            border-radius: 3px;
            background: #d3d3d3;
            outline: none;
            opacity: 1;
            -webkit-transition: .2s;
            transition: opacity .2s;
        }

        .slider:hover {
            opacity: 0.8;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 40px;
            height: 25px;
            border-radius: 10%;
            background: #099bdd;
            cursor: pointer;
        }

        .slider::-moz-range-thumb {
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #099bdd;
            cursor: pointer;
        }

        .buttonStyle1 {
            background-color: #099bdd;
            border: none;
            color: white;
            padding: 10px 24px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            border-radius: 8px;
            transition-duration: 0.4s;
            font-family: Arial, Helvetica, sans-serif
        }

        .buttonStyle2 {
            background-color: #DF0000;
            border: none;
            color: white;
            padding: 10px 24px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            border-radius: 8px;
            transition-duration: 0.4s;
            font-family: Arial, Helvetica, sans-serif
        }

        .buttonStyle3 {
            background-color: #616161;
            border: none;
            color: white;
            padding: 10px 24px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            border-radius: 8px;
            transition-duration: 0.4s;
            color: white;
            font-family: Arial, Helvetica, sans-serif
        }

        .buttonStyle4 {
            background-color: #ffffff;
            border: 2px solid #0097e9;
            color: white;
            padding: 10px 24px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            border-radius: 8px;
            transition-duration: 0.4s;
            color: black;
            font-family: Arial, Helvetica, sans-serif
        }

        .buttonStyle1:hover {
            background-color: #067fb7;
            color: white;
        }

        .buttonStyle2:hover {
            background-color: #FF0000;
            color: white;
        }

        .buttonStyle3:hover {
            background-color: #0097e9;
        }

        .buttonStyle4:hover {
            background-color: #0097e9;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        #snackbar {
            visibility: hidden;
            /* Hidden by default. Visible on click */
            min-width: 250px;
            /* Set a default minimum width */
            margin-left: -125px;
            /* Divide value of min-width by 2 */
            background-color: #333;
            /* Black background color */
            color: #fff;
            /* White text color */
            text-align: center;
            /* Centered text */
            border-radius: 2px;
            /* Rounded borders */
            padding: 16px;
            /* Padding */
            position: fixed;
            /* Sit on top of the screen */
            z-index: 1;
            /* Add a z-index if needed */
            left: 50%;
            /* Center the snackbar */
            bottom: 30px;
            /* 30px from the bottom */
        }

        /* Show the snackbar when clicking on a button (class added with JavaScript) */
        #snackbar.show {
            visibility: visible;
            /* Show the snackbar */
            /* Add animation: Take 0.5 seconds to fade in and out the snackbar.
  However, delay the fade out process for 2.5 seconds */
            -webkit-animation: fadein 0.5s, fadeout 0.5s 2.5s;
            animation: fadein 0.5s, fadeout 0.5s 2.5s;
        }

        @-webkit-keyframes fadein {
            from {
                bottom: 0;
                opacity: 0;
            }

            to {
                bottom: 30px;
                opacity: 1;
            }
        }

        @keyframes fadein {
            from {
                bottom: 0;
                opacity: 0;
            }

            to {
                bottom: 30px;
                opacity: 1;
            }
        }

        @-webkit-keyframes fadeout {
            from {
                bottom: 30px;
                opacity: 1;
            }

            to {
                bottom: 0;
                opacity: 0;
            }
        }

        @keyframes fadeout {
            from {
                bottom: 30px;
                opacity: 1;
            }

            to {
                bottom: 0;
                opacity: 0;
            }
        }

      iframe {
    display: block;       /* iframes are inline by default */
    background: #000;
    border: none;         /* Reset default border */
    height: 70vh;        /* Viewport-relative units */
    width: 90vw;
}

        .btn {
            width: 100%;
            margin: 5px;

        }

        .my-text {
            font-size: 20px;
            padding: 10px;
        }

        table,
        th,
        td {

            text-align: center;

            margin-bottom: 15px;
        }

        .my-div {
            margin-right: auto;
            margin-left: auto;
            margin-bottom: 15px;

        }

        .mydiv {
            margin-right: auto;
            margin-left: auto;
            display: flex;
            flex-direction: row;
            justify-content: center;
        }
    </style>

    <title>CCKIT Main</title>
</head>

<body>

    <div class="container">

        <div class="row">
            <div class="col-md-12 col-xs-12">
                <h1 class="text-center">CCKIT Main</h1>
            </div>
        </div>
       
      
        <hr>

        <div class="mydiv">
            <table>
                <tr>
                    <td style="vertical-align:middle;">
                        <b clabel id="camStatus" style="margin-right:30px"></b>
                    </td>
                    <td style="vertical-align:middle;">
                        <button onclick="flashToggle()" type="button" class="buttonStyle4">Flash Toggle</button>
                    </td>
                </tr>
            </table>


        </div>
        <div>
            <div style="position:relative;padding-top:56.25%;">
                <iframe id="iframe1" src="http://192.168.1.150/">
                    <p>Your browser does not support iframes.</p>
                </iframe>
            </div>
        </div>
        <hr>
        <div class="row">
            <div class="col-md-12 col-xs-12">
                <h5 id="lagText" class="text-center">lag</h5>
            </div>
        </div>
        <div class="row">

            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">Monitor</h2>
                </div>
                <hr>
                <table style="width:100%">
                    <tr>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Heater Status</td>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Humidifier Status</td>
                    </tr>
                    <tr>
                        <td id="heaterStatus" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>OFF
                        </td>
                        <td id="humidifierStatus" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>
                            OFF</td>
                    </tr>

                    <tr>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Current Temperature
                        </td>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Target Temperature</td>
                    </tr>

                    <tr>
                        <td id="tempVal" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>16</td>
                        <td id="tarTempVal" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>14
                        </td>
                    </tr>
                    <tr>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Current Humidity</td>
                        <td style='font-family: Arial, Helvetica, sans-serif;font-size: larger;'>Target Humidity</td>
                    </tr>
                    <tr>
                        <td id="humVal" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>16</td>
                        <td id="tarHumVal" style='font-family: Arial, Helvetica, sans-serif;font-size:x-large ;'>14</td>
                    </tr>
                </table>

                <div class="mydiv">
                    <h5 id="msg"></h5>
                </div>
                <div class="mydiv">
                    <button onclick="emgStop()" type="button" class="buttonStyle2">Emergency STOP</button>
                </div>

            </div>
        </div>
    </div>


    <div class="col-md-4 col-xs-12">
        <div class="col-md-12 col-xs-12">
            <h2 class="text-center">Controller</h2>
        </div>
        <hr>
        <div class="row">
            <div class="my-div">
                <h4 style="text-align:center;">Set Temperature</h4>
                <hr style="width:40%;text-align:left;margin-left:auto;margin-right:auto">
            </div>
            <div class="slidecontainer">
                <input id="r" class="slider" type="range" min="0" max="60" value="0" oninput="updateSliders()">
            </div>

            <div style="text-align:center;margin-left:auto;margin-right:auto" class="my-div">
                <h4 id="rVal">0C</h4>
            </div>
        </div>
        <div class="row">
            <div class="my-div">
                <h4 style="text-align:center;">Set Humidity</h4>
                <hr style="width:50%;text-align:left;margin-left:auto;margin-right:auto">
            </div>
            <div class="slidecontainer">
                <input id="g" class="slider" type="range" min="0" max="100" value="0" oninput="updateSliders()">
            </div>
            <div style="text-align:center;margin-left:auto;margin-right:auto" class="my-div">
                <h4 id="gVal">0%</h4>
            </div>


        </div>


        <button id="prcButton" onclick="sendRGB()" style="float: right;" type="button" class="buttonStyle1">Start
            Process</button>
        <button onclick="wifiConfigMode()" type="button" class="buttonStyle3">Start Wifi Config</button>

        <div class="my-div">
            <table style="width:100%;margin-top:30px;margin-bottom:30px">
                <tr>
                    <th style="font-size: 22px;">Kp=</th>
                    <th> <input type="text" style="width: 60px;" id="kpin" name="fname"></th>
                    <th style="font-size: 22px;">Ki=</th>
                    <th> <input type="text" id="kiin" style="width: 60px;" name="fname2"></th>
                    <th style="font-size: 22px;">Kd=</th>
                    <th> <input type="text" id="kdin" style="width: 60px;" name="fname3"></th>
                </tr>
            </table>
            <button onclick="savePID()" type="button" class="buttonStyle3">Save PID Settings</button>
            <button onclick="startCamUpdate()" type="button" class="buttonStyle3">Update Camera Firmware</button>
        </div>
    </div>



    <div id="snackbar">Some text some message..</div>
    <footer class="container footer">
        <div class="col-md-12 text-center">

        </div>
    </footer>

    <script>
        const d = new Date();
        let ms = d.getMilliseconds();
        var toastMessage;
        function emgStop() {
            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "emgStop?w=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
        function startCamUpdate() {
            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "camUpdate?w=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
        function savePID() {
            var kpVal = document.getElementById('kpin').value;
            var kiVal = document.getElementById('kiin').value;
            var kdVal = document.getElementById('kdin').value;
            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "savePID?p=" + kpVal + "&i=" + kiVal + "&d=" + kdVal;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
        function updateSliders() {
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            document.getElementById('rVal').innerHTML = String(redVal) + "C";
            document.getElementById('gVal').innerHTML = String(greenVal) + "%";
        }
        function sendRGB() {
            //get rgb values from sliders
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;


            // convert rgb values range from 0 - 100 to 0 - 255
            var r = parseInt(redVal).toString();
            var g = parseInt(greenVal).toString();

            // show slider value in labels
            document.getElementById('rVal').innerHTML = String(redVal) + "C";
            document.getElementById('gVal').innerHTML = String(greenVal) + "%";
            // document.getElementById('txt').innerHTML = String(ssidVal);

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "setRGB?r=" + r + "&g=" + g;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

        setInterval(function () {
            // Call a function repetatively with 2 Second interval
            getData();
            getMessage();
            notifyCam();
        }, 1000); //2000mSeconds update rate
        function notifyCam() {

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("camStatus").innerHTML =
                        this.responseText;
                    // console.log(this.responseText);
                }
            };
            xhttp.open("GET", "notifyCAM", true);
            xhttp.send();
        }
        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    const myArray = this.responseText.split("|");
                    document.getElementById("tempVal").innerHTML =
                        myArray[0];
                    document.getElementById("tarTempVal").innerHTML =
                        myArray[1];
                    document.getElementById("humVal").innerHTML =
                        myArray[2];
                    document.getElementById("tarHumVal").innerHTML =
                        myArray[3];
                    document.getElementById("kpin").placeholder =
                        myArray[4];
                    document.getElementById("kiin").placeholder =
                        myArray[5];
                    document.getElementById("kdin").placeholder =
                        myArray[6];
                    document.getElementById("heaterStatus").placeholder =
                        myArray[7];
                    document.getElementById("humidifierStatus").placeholder =
                        myArray[8];
                       let period =parseInt(myArray[9])-ms;
                    document.getElementById("lagText").innerHTML =
                     "Data Period:"+  period;
 ms=parseInt(myArray[9]);
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "liveData", true);
              console.log( xhttp.status);
            xhttp.send();

            var xhttp2 = new XMLHttpRequest();
            xhttp2.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("prcButton").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp2.open("GET", "readPrcButton", true);
            xhttp2.send();

            var xhttp3 = new XMLHttpRequest();
            xhttp3.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("snackbar").innerHTML =
                        this.responseText;

                    var x = document.getElementById("snackbar");

                    console.log("toast:" + this.responseText);
                    // Add the "show" class to DIV
                    x.className = "show";
                    if (this.responseText != "")
                        // After 3 seconds, remove the show class from DIV
                        setTimeout(function () { x.className = x.className.replace("show", ""); }, 3000);
                }

            };
            xhttp3.open("GET", "readToast", true);
            xhttp3.send();
        }
        function getMessage() {

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("msg").innerHTML =
                        this.responseText;
                    //  console.log(this.responseText);
                }
            };
            xhttp.open("GET", "message", true);
            xhttp.send();
        }
        function wifiConfigMode() {

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "setWifiMode?w=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
        function flashToggle() {

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {

                }
            };

            // create querrystring
            var sendStr = "flashToggle?w=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
    </script>

    <script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
        integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
        crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"
        integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa"
        crossorigin="anonymous"></script>
</body>

</html>,
)=====";
const char PAGE_404[] = R"====(
<html>
    <head>
        <title>404</title>
    </head>
    <body>
        <center><h1>ERROR CODE 404</h1></center>
        <center><h1>REQUESTED PAGE NOT FOUND</h1></center>
    </body>
</html>
)====";

const char SETTINGS_PAGE[] = R"=====(
<!doctype html>
<html lang="en">
<head>  
<meta name="viewport" content="width=device-width, initial-scale=1">  
<title> Login Page </title>  
<style>   
Body {  
  font-family: Calibri, Helvetica, sans-serif;  
  background-color: #099bdd;  
}  
.vertical-center {
  margin: 0;
  position: absolute;
  top: 50%;
  -ms-transform: translateY(-50%);
  transform: translateY(-50%);
}
button {   
       background-color: #185abc;   
       width: 50%;  
        color: white;   
        padding: 15px;   
          margin-top: 20px;
    margin-bottom: 20px;
    margin-right:auto;
    margin-left:auto;
        border: 1px solid black;   
        cursor: pointer;  
        border-radius: 6px; 
         }   
 form {   
        //border: 3px solid #f1f1f1;   

    }   
 input[type=text], input[type=password] {   
        width: 100%;   
        margin: 8px 0;  
        padding: 12px 20px;   
        display: inline-block;   
        border: 2px solid black;   
        box-sizing: border-box;   
    }  
 button:hover {   
        opacity: 0.7;   
    }   
  .cancelbtn {   
        width: auto;   
        padding: 10px 18px;  
        margin: 10px 5px;  
    }   
        
     
 .container {  
     margin: auto;
      width: 80%; 
        padding: 25px;   
        background-color: white;
         border-radius: 15px; 
         text-align: center;
    }   
    .mydiv {
    margin-right:auto;
    margin-left:auto;
     display: flex;
  flex-direction: row;
  justify-content: center;
}
</style>   
</head>    
<body>    
   
    <form>  
        <div class="container">   
         <center> <h1> CCKIT Wifi Settings </h1> </center>   
         <hr >
         <div class="mydiv">
          <b clabel id="camStatus"></b>
          </div>
           <hr  >
            <label>SSID</label>   
            <input type="text" placeholder="Enter SSID" id="ssidText" required>  
            <label>Password</label>   
            <input type="password" placeholder="Enter Password" id="passwordText" required>  
     <button type="submit"  onclick="sendRGB()">Process</button>   
     <button type="submit"  onclick="sendAPMode()">Start AP Mode</button>   
           <div class="mydiv">
          <h4 clabel id="msg"></h4>
          </div>
        </div>   
    </form>     
</body>     

    <script>
 function sendAPMode() {
            //get rgb values from sliders

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "startAP?s=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
        function sendRGB() {
            //get rgb values from sliders
            var passVal = document.getElementById('passwordText').value;
            var ssidVal = document.getElementById('ssidText').value;

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setWifi?s=" + ssidVal + "&p=" + passVal;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

           setInterval(function () {
            // Call a function repetatively with 2 Second interval
            notifyCam();
            getMessage();
        }, 1000); //2000mSeconds update rate

        function notifyCam() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("camStatus").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "notifyCAM", true);
            xhttp.send();
        }
         function getMessage() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("msg").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "message", true);
            xhttp.send();
        }
    </script>

    <script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
        integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
        crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"
        integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa"
        crossorigin="anonymous"></script>
</body>

</html>
)=====";
