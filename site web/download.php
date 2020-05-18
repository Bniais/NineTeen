<?php
	include('include/secure.php');

	$result = $link->query("SELECT count FROM nineteen_downloads");
	while ($row = $result->fetch_assoc()) { $compteur = $row["count"]; }

	if($_SERVER['REQUEST_METHOD'] == 'GET')
	{

		$file = $_GET["file"];
		$lancerTelechargement = FALSE;

		if($file == "mac")
		{
			$fichier = 'download/Nineteen_v1.1.1.zip';
			$fichier_taille = filesize($fichier);
			$lancerTelechargement = TRUE;
		}
		else if ($file == "windows")
		{
			$fichier = 'download/Nineteen_v1.1.1.exe';
			$fichier_taille = filesize($fichier);
			$lancerTelechargement = TRUE;
		}
		else if ($file == "linux")
		{?>
			<script language="javascript">
				alert("Bientot disponible");
			</script>
			<?php
		}

		if($lancerTelechargement)
		{
			header("Content-disposition: attachment; filename=$fichier");
			header("Content-Type: application/force-download");
			header("Content-Transfer-Encoding: application/octet-stream");
			header("Content-Length: $fichier_taille");
			header("Pragma: no-cache");
			header("Cache-Control: must-revalidate, post-check=0, pre-check=0, public");
			header("Expires: 0");
			readfile($fichier);

			// SEND REQUEST
			$result = $link->query("UPDATE nineteen_downloads SET count = count + 1");
		}

	}

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

			<div class="jeu" style="margin-left: 2%; float:left; width:60vw; height:33.75vw ">
					<iframe style="margin-top: 0.5%; margin-bottom: 0.5%; -moz-border-radius: 10px; -moz-border-radius:5%; -webkit-border-radius: 2%; border-radius: 2%;" width="98%"  height="98%" src="https://www.youtube.com/embed/AtzQSTAA0Kw" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen="1"></iframe>
			</div>

			<div class= "jeu" style="margin-right: 2%; float:right; width:30vw;">
				<h1>Liens de téléchargement</h1>
				<a href="download.php?file=mac"> <img style="width: 4vw; margin-right: 10%;" src="https://upload.wikimedia.org/wikipedia/commons/thumb/2/22/MacOS_logo_%282017%29.svg/768px-MacOS_logo_%282017%29.svg.png" alt="logo apple"></a>
				<a href="download.php?file=windows"><img style="width: 4vw; margin-right: 10%;" src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/ee/Windows_logo_–_2012_%28dark_blue%29.svg/768px-Windows_logo_–_2012_%28dark_blue%29.svg.png" alt="logo apple"></a>
			  <a href="download.php?file=linux"><img style="width: 4vw; " src="https://upload.wikimedia.org/wikipedia/commons/thumb/5/54/Ubuntu-Logo_ohne_Schriftzug.svg/1200px-Ubuntu-Logo_ohne_Schriftzug.svg.png" alt="logo apple"></a>
				<br>
				<br>
				<a>Compteur de téléchargement : <?php echo $compteur ?></a>
				<hr>
				<h2>Version actuelle :  <?php include("checkVersion.php"); echo $versionActuel ?></h2>
			</div>
			<div class= "jeu" style="margin-right: 2%; float:right; width:30vw;">
				<h1>Note de mise à jour</h1>
				<br>


				<h2>Version : 1.1.1</h2>
        <ul style="text-align: left;">
          <li>Flappy :</li>
            <ul>
								<li>Équilibrage du mode hard</li>
				        <li>Résolution de bug de score</li>
            </ul>
        </ul>
        <br>
        <br>
				<hr>

				<h2>Version : 1.1.0</h2>
                <ul style="text-align: left;">

                    <li>Room :</li>
                    <ul>
                        <li>Performances améliorées</li>
                        <li>Lumières améliorées</li>
                        <li>Amélioration des animations de zoom et dezoom</li>
                    </ul>
                    <li>Asteroid :</li>
                    <ul>
                        <li>Accelération du vaisseau plus puissante</li>
                        <li>Mode difficile disponible et terminé</li>
                    </ul>
                    <li>Démineur :</li>
                    <ul>
                        <li>Accès au jeu possible, sans score</li>
                    </ul>
                    <li>Shooter :</li>
                    <ul>
                        <li>Accès au jeu sans score et pas terminé</li>
                    </ul>
                </ul>
                <br>
                <br>
                <hr>

				<h2>Version : 1.0.1</h2>
					<ul style="text-align: left;">
						<li>Global :</li>
						<ul>
							<li>Résolution de probléme liée a l'affichage affectant une infime partie d'utilisateur</li>
							</ul>
					</ul>
					<br>
					<br>
					<hr>


				<h2>Version : 1.0.0</h2>
					<ul style="text-align: left;">
						<li>Global :</li>
						<ul>
							<li>Log disponibles dans les fichiers temporaires</li>
							<li>Chargement des textures des jeux pendant l'animation de zoom vers écran (plus de petit freeze au début)</li>
							<li>Chaque score est désormais affecté par un multiplicateur pour le classement général</li>
							<li>Mode plein écran et optimisation positionnement fenêtre</li>
							<li>Utilisation du gpu</li>
							</ul>

						<li>Launcher :</li>
						<ul>
							<li>Message indiquant pourquoi la connection a échoué</li>
							</ul>

						<li>Salle :</li>
						<ul>
						<li>Textures pour toute la salle</li>
							<li>Porte et couloir d'entrée</li>
							<li>Toilettes fonctionelles</li>
							<li>Animations de zoom vers les écrans plus précises</li>
							<li>Son provenant de plusieurs radios qu'il est possible de couper</li>
							<li>Un écran avec les classements se trouve sur le bar</li>
						</ul>

						<li>Flappy :</li>
						<ul>
							<li>Équilibrage entre les modes</li>
						</ul>

						<li>Tetris :</li>
						<ul>
							<li>Ajout de sons</li>
						</ul>

						<li>Snake :</li>
						<ul>
							<li>Ajout de sons</li>
						</ul>

						<li>Asteroid :</li>
						<ul>
							<li>Mode facile terminé (armes, munitions)</li>
							<li>Équilibrage</li>
							<li>Ajout de sons</li>
						</ul>

					</ul>
					<br>
					<br>
					<hr>


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

							echo  strip_tags( $row["message"] );
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

		</div>
	</body>
</html>



<?php $link->close(); ?>
