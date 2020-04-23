<?php
include('include/fonction.php');


if( $_SERVER['REQUEST_METHOD'] == 'POST' ){

	$sessionKey = $_POST["key"];
	$secureKey = $_POST["secure"];

	if( $sessionKey != '' &&  $secureKey != '')
	{
		if( secureCheck($secureKey,1) == 1 )
		{
			include('include/secure.php');


			$result = $link->query("SELECT userId FROM nineteen_session WHERE sessionKey = '$sessionKey' ");

			if ($result)
			{
				$id;
				while ($row = $result->fetch_assoc()) { $id = $row["userId"]; }


				$result = $link->query( "SELECT userId, FLOOR( SUM( score * multiplicator ) ) AS total FROM nineteen_scores NATURAL JOIN nineteen_multiplicators GROUP BY userId ORDER BY total DESC");

				if($result)
				{
					$count = 0;
					while ($row = $result->fetch_assoc()) {
						$count++;
						if( $id == $row["userId"] )
						{
							echo $row["total"];
							echo " ";
							echo $count;
						}
					}
					echo " / ";
					echo $count;

					// partie qui retourne top player pour un id jeux
					$result = $link->query("SELECT username,t1.gameId,t1.score FROM nineteen_scores AS t1 NATURAL JOIN nineteen_players WHERE t1.userId = ( SELECT userId FROM nineteen_scores AS t2 WHERE t1.gameId = t2.gameId ORDER BY t2.score DESC LIMIT 1 ) GROUP BY t1.gameId");
					if($result)
					{
						while ($row = $result->fetch_assoc()) {
							echo "  ";
							echo $row["username"];
							echo " ";
							echo $row["score"];
						}

					}


					$result = $link->query("SELECT gameId,score FROM `nineteen_session` NATURAL JOIN nineteen_scores WHERE sessionKey = '$sessionKey' ORDER BY gameId");
					if($result)
					{
						while ($row = $result->fetch_assoc()) {
							echo "  ";
							echo $row["score"];
						}
					}

					// RETOURNE LES MULTIPLICATEURS
					$result = $link->query("SELECT multiplicator FROM nineteen_multiplicators ORDER BY gameId");
					if($result)
					{
						while ($row = $result->fetch_assoc()) {
							echo "  ";
							echo $row["multiplicator"];
						}
					}

				}
				else
				{
					echo "-2"; // erreur connection
				}
			}



			$link->close();
		}
		else
		{
			// requete invalide cle de securite incorrect
			echo "-10";
		}
	}
	else
	{
		echo "-1"; // champs non complet
		echo "\n";
		echo $gameId;
		echo "\n";
		echo $score;
		echo "\n";
		echo $sessionKey;
		echo "\n";
		echo $secureKey;
	}

}

?>
