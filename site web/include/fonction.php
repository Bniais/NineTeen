<?php 

function getClientIP()
{
	if (array_key_exists('HTTP_X_FORWARDED_FOR', $_SERVER)){
		return  $_SERVER["HTTP_X_FORWARDED_FOR"];
	}else if (array_key_exists('REMOTE_ADDR', $_SERVER)) {
		return $_SERVER["REMOTE_ADDR"];
	}else if (array_key_exists('HTTP_CLIENT_IP', $_SERVER)) {
		return $_SERVER["HTTP_CLIENT_IP"];
	}
		return '';
}

function random($universal_key) 
{

    $string = "";

    $user_ramdom_key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^*()-_";
    srand((double)microtime()*time());
    for($i=0; $i<$universal_key; $i++) {
    $string .= $user_ramdom_key[rand()%strlen($user_ramdom_key)];
    }
    return $string;
}

function secure($seconde_temp) 
{
	$date = date("Y-m-d");
	$heure = date("H");
	$minute = date("i");
	$seconde = date("s");

	$seconde = $seconde - $seconde_temp;
	$b = $date . " JDlaliljasnc329832 " .$heure . " 0 " . $minute . " D(ancIjaa) " . $seconde; // $b contient "Bonjour Monde !"
	return md5($b);
}

function secureCheck($aTester ,$maxDelay)
{
    $verifier = 0;
    for ($i = 0; $i <= $maxDelay; $i++) {
        if ( $aTester == secure($i))
        {
            $verifier = 1;
        }
            
    }
    return $verifier;
}

?>
