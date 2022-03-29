<!DOCTYPE html>
<html>
<body>

<center>

<h3 style="color:rgb(0, 68, 255);font-family:verdana">Consumo en directo</h3>
<h4 id="label" style="color:rgb(0, 68, 255);font-family:verdana">???</h4>

<script type = "text/javascript">
    const serverUrl = "https://servidorappconsumo.goranpalacio.repl.co"

    function httpGet(theUrl) {
        var xmlHttpReq = new XMLHttpRequest();
        xmlHttpReq.open("GET", theUrl); 
        xmlHttpReq.send(null);
        return xmlHttpReq
    }

    setInterval(function () {
        const request = httpGet(serverUrl)
        request.onreadystatechange = function() {
            let number = parseFloat(request.responseText)
            number = Math.abs(number)
            document.getElementById("label").innerHTML = number.toString()
        }
    }, 1000);
</script>

</center>

</body>
</html>
