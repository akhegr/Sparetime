<?php
 function Connection(){
    if (!($link=mysql_connect("mydomain.com","username","password"))) {
       exit();
    }
    if (!mysql_select_db("username_db",$link)){
       exit();
    }
    return $link;
 }
 if(($_GET["temp"] < "-15") OR ($_GET["temp"] > "35"))
 {
    echo("The temperature isn't within my interval. It must be higher than -15 [℃], and lower than 35 [℃].");
 }
 elseif($_GET["key"]=="xxxxxxxx")
 {
    $link = Connection();
    $time = date("Y-m-d H:i:s");
    $Sql = "insert into vejrstation (datetime, temperature)  values ('$time', '" . $_GET["temp"] . "')";
    mysql_query($Sql, $link);
    echo ("The entered key is correct, and the data is now added to the database.");
 }
 else	//http://passwordsgenerator.net
 {
    echo("The key isn't entered, or isn't correct.");
 }
?>
