export default `<meta name="viewport" content="width=device-width, initial-scale=1.0">
<html>
    <head>
        <link rel="stylesheet" type="text/css" href="./css/style.css">
    </head>
    <div class="container">  
      
      <form id="loginForm" action="#">
        <br>
        <fieldset>
          <input id="name" placeholder="Your name" type="text" tabindex="1" required autofocus>
        </fieldset>
        <fieldset>
          <input id="email" placeholder="Your Email Address" type="email" tabindex="2" required>
        </fieldset>
        <fieldset>
          <input id="company" placeholder="Your Company (optional)" type="text" tabindex="3">
        </fieldset>
        <fieldset>
          <button type="submit" >Log in to WiFi</button>
        </fieldset>
      </form>
    </div>
    <hr>
</html>`