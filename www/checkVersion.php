<?php
	$versionActuel = "0.1.4b";
<<<<<<< HEAD
	$versionBeta = "0.1.5b-dev";
=======
	$versionBeta = "0.1.5b";
>>>>>>> 2ebaaa07c07b26375af84038b79557f27565933f

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{
		if ( ($_POST["version"] == $versionActuel) || ($_POST["version"] == $versionBeta) )
			echo "1";
		else
			echo "0";

	}
?>
