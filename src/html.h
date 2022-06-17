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
            iframe {
        width: 100%;
        height: 100%;
        border: none;
      }
        .btn {
            width: 100%;
            margin: 5px;
        }

        .my-text {
            font-size: 20px;
            padding: 10px;
        }
table, th, td {

    text-align: center;
     font-family: Arial, Helvetica, sans-serif;
     margin-bottom: 15px;
}
        .my-div {
            margin-bottom: 15px;

        }
           .mydiv {
    margin-right:auto;
    margin-left:auto;
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
                <button onclick="flashToggle()"  type="button" class="btn btn-primary">Flash Toggle</button>
        </td>
    </tr>
</table>
         
      
          </div>
          <div>
          <div style="position:relative;padding-top:56.25%;">
         <iframe id="iframe1" src="http://192.168.1.150/" frameborder="0" allowfullscreen style="position:absolute;top:0;left:0;width:100%;height:100%;">
  <p>Your browser does not support iframes.</p>
</iframe>
</div>
</div>
<hr>
        <div class="row">

           

            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">Controller</h2>
                </div>
                <hr>
                <div class="row">
                  <div class="col-md-8 col-xs-8 div-my">


                 <p style="text-align:center">Target Temperature</p>
                </div>
                    <div class="col-md-8 col-xs-8 my-div">
                     
                        <input id="r" type="range" min="0" max="60" value="0" onchange="updateSliders()">
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="rVal">0%</span>
                    </div>
                </div>
                <div class="row">
               <div class="col-md-8 col-xs-8 div-my">
                <p style="text-align:center">Target Humidity</p>
                </div>
                    <div class="col-md-8 col-xs-8 my-div">
                        <input id="g" type="range" min="0" max="100" value="0"onchange="updateSliders()">
                        
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="gVal">0%</span>
                    </div>
                </div>

                    
                    <button id="prcButton" onclick="sendRGB()" type="button" class="btn btn-primary">Process</button>
                    <button onclick="wifiConfigMode()" type="button" class="btn btn-primary">Start Wifi Config</button>
                   
                </div>

               
             <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">Monitor</h2>
                </div>
                 <hr>

                                  <table style="width:100%">
  <tr>
    <td style='font-weight:bold;'>Current Temperature</td>
    <td style='font-weight:bold;'>Target Temperature</td>
  </tr>
  <tr>
    <td  id="tempVal">16</td>
    <td id="tarTempVal">14</td>
  </tr>
</table>
                <table style="width:100%">
  <tr>
    <td style='font-weight:bold;'>Current Humidity</td>
    <td style='font-weight:bold;'>Target Humidity</td>
  </tr>
  <tr>
    <td  id="humVal">16</td>
    <td id="tarHumVal">14</td>
  </tr>
</table> 
                 <div class="mydiv"> 
                    <h5 id="msg" ></h5>
                      </div>
               
            </div>
        </div>
    </div>

    <footer class="container footer">
        <div class="col-md-12 text-center">

        </div>
    </footer>

    <script>

function updateSliders()
{
     var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            document.getElementById('rVal').innerHTML = String(redVal)+"C";
            document.getElementById('gVal').innerHTML = String(greenVal)+"%";
}
        function sendRGB() {
            //get rgb values from sliders
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;


            // convert rgb values range from 0 - 100 to 0 - 255
            var r = parseInt(redVal ).toString();
            var g = parseInt(greenVal ).toString();

            // show slider value in labels
            document.getElementById('rVal').innerHTML = String(redVal)+"C";
            document.getElementById('gVal').innerHTML = String(greenVal)+"%";
           // document.getElementById('txt').innerHTML = String(ssidVal);

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setRGB?r=" + r + "&g=" + g ;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

        setInterval(function () {
            // Call a function repetatively with 2 Second interval
            getData();
            getMessage() ;
            notifyCam();
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
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "liveData", true);
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

</html>
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
           <div class="mydiv">
          <h4 clabel id="msg"></h4>
          </div>
        </div>   
    </form>     
</body>     

    <script>

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



