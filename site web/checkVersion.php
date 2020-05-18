<?php
	$versionActuel = "1.1.2";
	$versionBeta = "1.1.3-dev";

	$versionAutoriser[0] =  $versionBeta;
	$versionAutoriser[1] =  $versionActuel;

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{

		$versionTest = $_POST["version"];

		$autoriserConnexion = 0;
		foreach ($versionAutoriser as $value) {
			if( $versionTest == $value )
				$autoriserConnexion = 1;
		}

		echo $autoriserConnexion;
	}



?>
