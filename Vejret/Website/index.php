<!DOCTYPE html>
<html>
<head>
	<title>New document</title>
	<meta name="generator" content="TSW WebCoder 2013">
	<script type="text/javascript" src="https://www.google.com/jsapi?autoload={"modules":[{"name":"visualization","version":"1.1","packages":["corechart"]}]}"></script>
   		
	<script>
		      google.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
		["Tidspunkt", "Temperatur"],
			<?php
// Connect to database server
	mysql_connect("mydomain.com", "user", "password") or die (mysql_error());
	// Select database
	mysql_select_db("user_db") or die(mysql_error());
	// SQL query
	$strSQL = "(SELECT * FROM vejrstation ORDER BY id DESC limit 0,6) order by id asc";
	// Execute the query (the recordset $rs contains the result)
	$rs = mysql_query($strSQL);
	
	// Loop the recordset $rs
	// Each row will be made into an array ($row) using mysql_fetch_array
	while($row = mysql_fetch_array($rs)) {		
        echo ("["". $row["datetime"] ."", ". $row["temperature"] ."],");
	}
	
	// Close the database connection
	mysql_close();
?>
        ]);
        var options = {
          title: "Temperatur i Aaby",
		  legend: "none",
          hAxis: {title: "Tidspunkt",  titleTextStyle: {color: "#333"}},
          vAxis: {minValue: 15,maxValue: 25,title: "Temperatur [\xB0C]",  titleTextStyle: {color: "#333"}}
        };
        var chart = new google.visualization.AreaChart(document.getElementById("chart_div"));
        chart.draw(data, options);
      }
	</script>
</head>
<body>
	<div id="chart_div" style="width: 480px; height: 500px;"></div>
</body>
</html>
