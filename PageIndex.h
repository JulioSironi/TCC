const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SER</title>
</head>
<body>
    <h1>
        <p>Escreva sua mensagem</p>
    </h1>
    <form>
        <textarea name="message"cols="30" rows="10" id="recado"></textarea>
        <br>
        <p>Senha:</p>
        <input type="password" id="senha">
        <br><br><br>
        <div> 
            <input type="datetime" id="data">
        </div>
        <br><br><br>
        <div>
            <button type="button" class="buttonSend" onclick="sendData()">Enviar Mensagem</button>
        </div> 
        <br><br><br><br>
        <input type="reset">
    </form>
    <script>
        function sendData() {
            var senha = document.getElementById("senha").value;
            if(senha === 'senha'){
                var Text = document.getElementById("recado").value;
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "setText?TextContents="+Text, true);
                xhttp.send(); 
                var data = document.getElementById("data").value
                var http = new XMLHttpRequest();
                http.open("GET", "setData?DataContents="+data, true);
                http.send();
                alert ("Mensagem enviada!");
            }
            else{
                alert("Senha Incorreta!");
            }
            
        }
    </script>
</body>
</html>


)=====";
