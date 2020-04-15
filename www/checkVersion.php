<?php
	$versionActuel = "0.1.4b";
	$versionBeta = "0.1.5b-dev";

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{
		if ( ($_POST["version"] == $versionActuel) || ($_POST["version"] == $versionBeta) )
			echo "1";
		else
			echo "0";

	}
?>
