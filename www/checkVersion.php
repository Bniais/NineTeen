<?php
	$versionActuel = "0.1.4b";
	$versionBeta = "0.1.5b-dev";
	$versionApush = "1.0.0";

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{
		if ( ($_POST["version"] == $versionActuel) || ($_POST["version"] == $versionBeta)  || ($_POST["version"] == $versionApush) )
			echo "1";
		else
			echo "0";
	}
?>
