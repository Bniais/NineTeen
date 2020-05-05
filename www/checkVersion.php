<?php
	$versionActuel = "1.0.0";
	$versionBeta = "1.0.1-dev";

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{
		if ( ($_POST["version"] == $versionActuel) || ($_POST["version"] == $versionBeta)  || ($_POST["version"] == $versionApush) )
			echo "1";
		else
			echo "0";
	}
?>
