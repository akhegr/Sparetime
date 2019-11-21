<?php
	$key = $_GET["key"];
	$angle = $_GET["dir"];
	$windSpeed = $_GET["speed"];
	$rainAmount = $_GET["amount"];
	$temp = $_GET["temp"];

    if($key!="D9ef") {
        echo("Error: The key isn't entered, or isn't correct.");
    }
	elseif($windDir < 0) {
		echo("Error: Windspeed is incorrect!");
	}
	elseif($windSpeed < 0) {
		echo("Error: Windspeed is incorrect!");
	}
	elseif($rainAmount < 0) {
		echo("Error: Rain amount is incorrect!");
	}
	else {		//http://passwordsgenerator.net 
		$con=mysqli_connect("mysql17.unoeuro.com","akselhg_dk","deluxscans770","akselhg_dk_db") or die("SQL-login error");
		// Check connection
		if (mysqli_connect_errno()) {
			echo "Failed to connect to MySQL: " . mysqli_connect_error();
		}

		// Perform queries 
		//mysqli_query($con,"SELECT * FROM Persons");
		$time = date("Y-m-d H:i:s");
		
		switch($angle)
		{
			case 0:
			case 1:
	       		$windDir	= "N";
				break;
	       	case 2:
				$windDir	= "NE";
	       		break;
	       	case 3:
	       	case 4:
			case 5:
				$windDir	= "E";
	       		break;    
	     	case 6:
				$windDir	= "SE";
	       		break;    
	     	case 7:
	     	case 8:
	     	case 9:
				$windDir	= "S";
	       		break;
	     	case 10:
				$windDir	= "SW";
	       		break;
	     	case 11:
	     	case 12:
				$windDir	= "W";
	       		break;
	     	case 13:
	     	case 14:
				$windDir	= "NW";
	       		break;
		 	case 15:
				$windDir	= "N";
	       		break;
		 	default:
				$windDir	= "unknown";
	       		break;
		}
		
		$result = mysqli_query($con, "INSERT INTO newWeather (datetime, windDir, windSpeed, rainAmount, temperature)  VALUES ('$time', '$windDir', '$windSpeed', '$rainAmount', '$temp')") or trigger_error("Query Failed! SQL: $sql - Error: ".mysqli_error(), E_USER_ERROR); //or die(mysqli_error($db));
		if($result)
		{
			echo ("The entered key is correct, and the data is now added to the database.");
		}
		else
		{
			echo "Insertion error";
		}
		
		mysqli_close($con);
	}
?>