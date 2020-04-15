<?php
 if ( $_SERVER['REQUEST_METHOD'] == 'GET' )
{
	secure_temp(0);
	echo "0 ------ <br>";
	echo secure(0);
	echo "<br> 1 ------ <br>";
	echo secure(1);
	echo "<br>2 ------ <br>";
	echo secure(2);
	echo "<br>3 ------ <br>";
	echo secure(3);
	echo "<br>4 ------ <br>";
	echo secure(4);
	echo "<br>5 ------ <br>";
	echo secure(5);
	echo "<br>6 ------ <br>";
	echo secure(6);
	echo "<br>7 ------ <br>";
	echo secure(7);
	echo "<br>8 ------ <br>";
	echo secure(8);
	echo "<br>9 ------ <br>";
	echo secure(9);
	echo "<br>10 ------ <br>";
	echo secure(10);
}

if ( $_SERVER['REQUEST_METHOD'] == 'POST' ){

	$key = $_POST["key"];
	$username = $_POST["username"];
	$password = md5($_POST["pwd"]);
	$secure = $_POST["secure"];

	if ( $secure == secure(0) )
	{
		$secureVerified = 1;

	}
	else if ( $secure == secure(1))
	{
		$secureVerified = 2;

	}
	else {
		$secureVerified = 0;
		echo "-5";
		exit;
	}

	if( $key != '' && $secureVerified != 0 )
	{
		include('include/secure.php');
		if ($result = $link->query("SELECT userId FROM nineteen_session WHERE sessionKey = '$key'")) {
			if($result->num_rows == 1){
				echo "SUCCESS";
			}
			else
				echo "-3"; // incorrect key
		}
		else
			echo "-2"; // error with connect
		/* Libération du jeu de résultats */
		$link->close();
		exit;

	}
	else if ( $username != '' && $password != '' && $secureVerified != 0)
	{
		include('include/secure.php');
		if ($result = $link->query("SELECT userId FROM nineteen_players WHERE username = '$username' AND password = '$password'")) {
			if($result->num_rows == 1){
				$user = $result->fetch_assoc();
				$result->close();
				$userId= $user['userId'];

				if ($result = $link->query("SELECT * FROM nineteen_session WHERE userId = '$userId'")) {

					$sessionKey = random(256);
					if($result->num_rows == 0){
						// cration d'une clé

						if ($result = $link->query("INSERT INTO nineteen_session (userId,sessionKey) VALUES ('$userId','$sessionKey')")){
							if ($result)
								echo $sessionKey;

						}
					}
					else
					{
						// destruction de l'ancienne clé
						if ($result = $link->query("UPDATE nineteen_session set sessionKey = '$sessionKey' WHERE userId='$userId'")){
							if ($result)
								echo $sessionKey;

						}
					}


				}
				else {
					echo "echec log";
				}

			}
			else
			{
				echo "-3";
			}

		}
		else {
			echo "-2"; // connexion impossible
		}
		/* Libération du jeu de résultats */
		$link->close();
	}
	else
	{
		echo "-1"; // champs manquant
		exit;
	}
}


function getClientIP(){
	if (array_key_exists('HTTP_X_FORWARDED_FOR', $_SERVER)){
		return  $_SERVER["HTTP_X_FORWARDED_FOR"];
	}else if (array_key_exists('REMOTE_ADDR', $_SERVER)) {
		return $_SERVER["REMOTE_ADDR"];
	}else if (array_key_exists('HTTP_CLIENT_IP', $_SERVER)) {
		return $_SERVER["HTTP_CLIENT_IP"];
	}
		return '';
	}

function random($universal_key) {

		$string = "";

		$user_ramdom_key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^*()-_";
		srand((double)microtime()*time());
		for($i=0; $i<$universal_key; $i++) {
		$string .= $user_ramdom_key[rand()%strlen($user_ramdom_key)];
		}
		return $string;
		}

function secure($seconde_temp) {
	$date = date("Y-m-d");
	$heure = date("H");
	$minute = date("i");
	$seconde = date("s");

	$seconde = $seconde - $seconde_temp;
	//$chaine = str_cat( $date, ' ' ,$heure,':',$minute,':',$seconde );
	$b = $date . " JDlaliljasnc329832 " .$heure . " 0 " . $minute . " D(ancIjaa) " . $seconde; // $b contient "Bonjour Monde !"
	return md5($b);
}

function secure_temp($seconde_temp) {
	$date = date("Y-m-d");
	$heure = date("H");
	$minute = date("i");
	$seconde = date("s");

	$seconde = $seconde - $seconde_temp;
	//$chaine = str_cat( $date, ' ' ,$heure,':',$minute,':',$seconde );
	$b = $date . " JDlaliljasnc329832 " .$heure . " 0 " . $minute . " D(ancIjaa) " . $seconde; // $b contient "Bonjour Monde !"
	echo "\n --------";
	echo md5($b);
	echo "\n";
	return md5($b);
}
?>
