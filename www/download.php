<?php
	include('include/secure.php');

	if($_SERVER['REQUEST_METHOD'] == 'POST')
	{
		$message = $_POST["message"];
		if($message != "")
		{
			$message = $link->real_escape_string($message);
			$yourIP = getClientIP();
			$resultat = $link->query("INSERT INTO nineteen_tickets (statut,message,ip) VALUES (0,'$message','$yourIP') ");
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

?>

<!DOCTYPE html>
<html>
	<?php
	$titre = "Nineteen | Téléchargement";
	 include('header.php');
	?>

	<body>
		<?php include('navigation.php'); ?>
		<h1>Téléchargement</h1>
		<br><br>
		<div class="container">
			<div class= "jeu" style="margin-left: 2%; float:left; width:60vw;">
				<h1>Feedback</h1>
				<div class= "jeu" style="margin-left: 2%; float:left; width:30vw;">
					<h2>Last feedback</h2>

					<?php
						$max = -1;
						$resultat = $link->query("SELECT * FROM nineteen_tickets ORDER BY id DESC");
						while ($row = $resultat->fetch_assoc() ) {

							echo "<hr>";
							echo "<br>";
							echo "<a style=\"margin-left: -55%;\">#";
							echo  $row["id"];

							if( $row["statut"] == 1)
								echo "<a> <a style=\"margin-left: 10%;\">DONE<a><br><br><a>";
							else
								echo "<a> <a style=\"margin-left: 10%;\">IN PROGRESS<a><br><br><a>";

							echo  $row["message"];
							echo "</a><br><br>";
							if($max == -1)
								$max = $row["id"];
						}

						$max++;
					?>

				</div>
				<div class= "jeu" style="margin-left: 2%; float:left; width:25vw;">
					<h2>Créer un ticket</h2>
					<form action="#" method="post">
						<input class="ticket-num" type="text" placeholder="#<?php echo $max ?>" disabled="true">
						<br>
						<textarea class="ticket-message" name="message" rows=10 COLS=40 placeholder="Message"></textarea>
						<br>

						<input class="ticket-send" type="submit" value="Envoyer">
					</form>
					<br>
				</div>
			</div>

			<div class= "jeu" style="margin-left: 2%; float:left; width:30vw;">
				<h1>Liens de téléchargement</h1>
				<a href="download/Nineteen_v0.1.4b.dmg"> <img style="width: 4vw; margin-right: 10%;" src="https://upload.wikimedia.org/wikipedia/commons/thumb/2/22/MacOS_logo_%282017%29.svg/768px-MacOS_logo_%282017%29.svg.png" alt="logo apple"></a>
				<a href="download/Nineteen_v0.1.4b.exe"><img style="width: 4vw; margin-right: 10%;" src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/ee/Windows_logo_–_2012_%28dark_blue%29.svg/768px-Windows_logo_–_2012_%28dark_blue%29.svg.png" alt="logo apple"></a>
				<a href="#"><img style="width: 4vw; " src="https://upload.wikimedia.org/wikipedia/commons/thumb/5/54/Ubuntu-Logo_ohne_Schriftzug.svg/1200px-Ubuntu-Logo_ohne_Schriftzug.svg.png" alt="logo apple"></a>
				<br>
				<br>
				<hr>
				<h2>Version actuelle :  <?php include("checkVersion.php"); echo $versionActuel ?></h2>
			</div>
			<div class= "jeu" style="margin-left: 2%; float:left; width:30vw;">
				<h1>Note de mise à jour</h1>
				<br>

				<h2>Version : 0.1.4b</h2>
				<ul style="text-align: left;">
					<li>Global :</li>
					<ul>
						<li>L'envoi des scores ne fait plus freeze le jeu</li>
						<li>Animation de chargement d'envoi des scores et de succès/échec d'envoi dans les jeux</li>
						<li>Connexion maintenue sur Windows quand on quitte le jeu</li>
					</ul>
					<li>Launcher :</li>
					<ul>
						<li>Curseur d'insertion de texte</li>
						<li>Fix bug d'écriture de chiffres</li>
						<li>Seuls les chiffres et lettres peuvent être rentrées</li>
						<li>Animation de chargement pendant la requête de connexion</li>
						<li>Animation des boutons (au survol et au clic)</li>
					</ul>
					<li>Salle :</li>
					<ul>
						<li>Possibilité de courir avec shift</li>
						<li>Affichage du record personnel</li>
					</ul>
					<li>Flappy :</li>
					<ul>
						<li>Fix hitbox à l'entrée et sortie de tuyaux</li>
					</ul>
					<li>Snake :</li>
					<ul>
						<li>Hitbox moins sévère</li>
						<li>Fix crash de digestion</li>
						<li>Équilibrage</li>
					</ul>
					<li>Tetris :</li>
					<ul>
						<li>Toutes les pièces apparaissent au milieu de la grille en haut</li>
					</ul>
					<li>Asteroid :</li>
					<ul>
						<li>Tab pour changer d'armes (Lance-missile et laser ne sont pas finis)</li>
					</ul>

				</ul>
				<br>
				<br>
				<hr>


				<h2>Version : 0.1.3b</h2>
				<ul style="text-align: left;">
					<li>Global :</li>
					<ul>
						<li>La musique du launcher tourne désormais en boucle</li>
						<li>Fix quelques erreurs de connections au serveur sous windows	</li>
					</ul>
					<li>Flappy :</li>
					<ul>
						<li>On peut désormais quitter le jeu flappy avec echap et en quittant la fenêtre à tout moment</li>
						<li>Le score ne gêne plus 	</li>
					</ul>
					<li>Tetris :</li>
					<ul>
						<li>Changements d'équilibrage de points (et correction de bugs)</li>
						<li>Changements du bonus FILL</li>
					</ul>

				</ul>
				<br>
				<br>
				<hr>
				<h2>Version : 0.1.2b</h2>
				<ul style="text-align: left;">
					<li>Global :</li>
					<ul>
						<li>Ajout d'un installateur</li>
						<li>Passage des sons de stéréo à mono</li>
						<li>Supression des DLLs en trop dans la version Windows</li>
					</ul>
					<li>Flappy :</li>
					<ul>
						<li>bug animation accès borne flappy</li>
					</ul>
				</ul>
				<br>



			</div>

		</div>
	</body>
</html>



<?php $link->close(); ?>
