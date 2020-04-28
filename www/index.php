<?php


	$recherche = 0;
	include('include/secure.php');

	if($_SERVER['REQUEST_METHOD'] == 'GET')
	{
		$nomRecherche = $_GET["joueur"];
		if( $nomRecherche != "")
		{
			$scoreRecherche[16];
			$classmentRecherche[16];

			if ( $resultat = $link->query("SELECT FLOOR( SUM( score * multiplicator ) ) AS total FROM nineteen_scores NATURAL JOIN nineteen_players NATURAL JOIN nineteen_multiplicators WHERE username = '$nomRecherche' GROUP BY userId ORDER BY total DESC LIMIT 3") )
				{
					while ($row = $resultat->fetch_assoc() ) {
						$scoreRecherche[0] =  $row["total"];
					}


					if( $resultat = $link->query("SELECT COUNT(*) FROM (SELECT username, FLOOR( SUM( score * multiplicator ) ) AS total FROM nineteen_scores NATURAL JOIN nineteen_players NATURAL JOIN nineteen_multiplicators GROUP BY userId ) AS s WHERE s.total > $scoreRecherche[0]") )
					{

						while ($row = $resultat->fetch_assoc() ) {
							$classmentRecherche[0] = $row["COUNT(*)"] + 1;
						}

					}
					$scoreRecherche[0] = number_format( $scoreRecherche[0] , 0, ',', ' ');
				}



			// RECUPERER 3 MEILLEURE JOUEUR / JEUX
			for ($i = 1; $i < 16 ; $i++) {
				if ( $resultat = $link->query("SELECT username,score FROM `nineteen_players` NATURAL JOIN nineteen_scores WHERE gameId = $i AND username = '$nomRecherche' ORDER BY score DESC LIMIT 3") )
				{
					while ( $row = $resultat->fetch_assoc()) {
						$scoreRecherche[$i] = $row["score"];

						if( $resultat = $link->query("SELECT COUNT(*) FROM nineteen_scores WHERE score > $scoreRecherche[$i] AND gameId = $i") )
							{
								while ($row = $resultat->fetch_assoc() ) {
									$classmentRecherche[$i] = $row["COUNT(*)"] + 1;
							}

						}
					}
				}
			}
			$recherche = 1;
		}
	}

	$nom[16][3];
	$score[16][3];


	// RECUPERER 3 MEILLEURS JOUEURS

	if ( $resultat = $link->query("SELECT username, FLOOR( SUM( score * multiplicator ) ) AS total FROM nineteen_scores NATURAL JOIN nineteen_multiplicators NATURAL JOIN nineteen_players GROUP BY userId ORDER BY total DESC LIMIT 3") )
		{
			$indice = 0;
			while ($row = $resultat->fetch_assoc() ) {

				$nom[0][$indice] =  $row["username"];

				$score[0][$indice++] =  number_format( $row["total"] , 0, ',', ' ');
			}
		}



	// RECUPERER 3 MEILLEURE JOUEUR / JEUX
	for ($i = 1; $i < 16 ; $i++) {
		if ( $resultat = $link->query("SELECT username,score FROM `nineteen_players` NATURAL JOIN nineteen_scores WHERE gameId = $i ORDER BY score DESC LIMIT 3") )
		{
			$indice = 0;


			while ( $row = $resultat->fetch_assoc()) {
				$nom[$i][$indice] =  $row["username"];
				$score[$i][$indice++] =  number_format( $row["score"] , 0, ',', ' ');
			}
		}
	}

	$link->close();

?>





<!DOCTYPE html>
<html>

	<?php
	$titre = "Nineteen | Accueil";
	 include('header.php');
	?>

	<body>
	<?php include('navigation.php'); ?>

	<h1>MEILLEUR JOUEUR</h1>
	<h1 class="blink leader" >  <?php echo $nom[0][0]; ?>  </h1>
	<br>
	<br>
	<h1>CLASSEMENT</h1>
	<center>
		<form action="#" method="GET">
				<input class="recherche" type="text" placeholder="Joueur" name="joueur">
				<input class="send-rechercher" type="submit" value="Rechercher">
				<br>
				<br>
				<br>
			</form>
	</center>

	<div>

		<div class="container">

			<div class="classement center">


				<h1>Classement general</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[0][0]; ?></a> <a class="rang-score"><?php echo $score[0][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[0][1]; ?></a> <a class="rang-score"><?php echo $score[0][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[0][2]; ?></a> <a class="rang-score"><?php echo $score[0][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[0]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[0]; ?></a>
				</div>
				<?php } ?>
			</div>

		</div>


		<div class="container">
			<div class="jeu left">


				<h1>Flappy Easy</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[12][0]; ?></a> <a class="rang-score"><?php echo $score[12][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[12][1]; ?></a> <a class="rang-score"><?php echo $score[12][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[12][2]; ?></a> <a class="rang-score"><?php echo $score[12][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[12]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[12]; ?></a>
				</div>
				<?php } ?>
				<br>
				<hr>
				<h1>Flappy Hard</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[1][0]; ?></a> <a class="rang-score"><?php echo $score[1][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[1][1]; ?></a> <a class="rang-score"><?php echo $score[1][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[1][2]; ?></a> <a class="rang-score"><?php echo $score[1][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[1]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[1]; ?></a>
				</div>
				<?php } ?>


			</div>

			<div class="jeu center">


				<h1>Tetris Easy</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[11][0]; ?></a> <a class="rang-score"><?php echo $score[11][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[11][1]; ?></a> <a class="rang-score"><?php echo $score[11][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[11][2]; ?></a> <a class="rang-score"><?php echo $score[11][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[11]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[11]; ?></a>
				</div>
				<?php } ?>
				<br>
				<hr>
				<h1>Tetris Hard</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[2][0]; ?></a> <a class="rang-score"><?php echo $score[2][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[2][1]; ?></a> <a class="rang-score"><?php echo $score[2][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[2][2]; ?></a> <a class="rang-score"><?php echo $score[2][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[2]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[2]; ?></a>
				</div>
				<?php } ?>

			</div>


			<div class="jeu right">


				<h1>Asteroid Easy</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[10][0]; ?></a> <a class="rang-score"><?php echo $score[10][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[10][1]; ?></a> <a class="rang-score"><?php echo $score[10][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[10][2]; ?></a> <a class="rang-score"><?php echo $score[10][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[10]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[10]; ?></a>
				</div>
				<?php } ?>
				<br>
				<hr>
				<h1>Asteroid Hard</h1>
				<div class="player1 blink">
					<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[3][0]; ?></a> <a class="rang-score"><?php echo $score[3][0]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[3][1]; ?></a> <a class="rang-score"><?php echo $score[3][1]; ?></a>
				</div>
				<div class="player2">
					<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[3][2]; ?></a> <a class="rang-score"><?php echo $score[3][2]; ?></a>
				</div>
				<?php if( $recherche ) { ?>
				<div class="player2">
					<a class="rang-classement "><?php echo $classmentRecherche[3]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[3]; ?></a>
				</div>
				<?php } ?>


			</div>

		</div>

	<div class="container">
		<div class="jeu left">


			<h1>Pacman Easy</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[9][0]; ?></a> <a class="rang-score"><?php echo $score[9][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[9][1]; ?></a> <a class="rang-score"><?php echo $score[9][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[9][2]; ?></a> <a class="rang-score"><?php echo $score[9][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[9]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[9]; ?></a>
			</div>
			<?php } ?>

			<br>
			<hr>
			<h1>Pacman Hard</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[4][0]; ?></a> <a class="rang-score"><?php echo $score[4][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[4][1]; ?></a> <a class="rang-score"><?php echo $score[4][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[4][2]; ?></a> <a class="rang-score"><?php echo $score[4][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[4]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[4]; ?></a>
			</div>
			<?php } ?>


		</div>

		<div class="jeu center">


			<h1>Snake Easy</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[8][0]; ?></a> <a class="rang-score"><?php echo $score[8][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[8][1]; ?></a> <a class="rang-score"><?php echo $score[8][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[8][2]; ?></a> <a class="rang-score"><?php echo $score[8][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[8]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[8]; ?></a>
			</div>
			<?php } ?>

			<br>
			<hr>
			<h1>Snake Hard</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[5][0]; ?></a> <a class="rang-score"><?php echo $score[5][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[5][1]; ?></a> <a class="rang-score"><?php echo $score[5][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[5][2]; ?></a> <a class="rang-score"><?php echo $score[5][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[5]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[5]; ?></a>
			</div>
			<?php } ?>


		</div>


		<div class="jeu right">


			<h1>Demineur Easy</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[7][0]; ?></a> <a class="rang-score"><?php echo $score[7][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[7][1]; ?></a> <a class="rang-score"><?php echo $score[7][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[7][2]; ?></a> <a class="rang-score"><?php echo $score[7][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[7]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[7]; ?></a>
			</div>
			<?php } ?>
			<br>
			<hr>
			<h1>Demineur Hard</h1>
			<div class="player1 blink">
				<a class="rang-classement ">1</a> <a class="rang-name "><?php echo $nom[6][0]; ?></a> <a class="rang-score"><?php echo $score[6][0]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">2</a> <a class="rang-name "><?php echo $nom[6][1]; ?></a> <a class="rang-score"><?php echo $score[6][1]; ?></a>
			</div>
			<div class="player2">
				<a class="rang-classement ">3</a> <a class="rang-name "><?php echo $nom[6][2]; ?></a> <a class="rang-score"><?php echo $score[6][2]; ?></a>
			</div>
			<?php if( $recherche ) { ?>
			<div class="player2">
				<a class="rang-classement "><?php echo $classmentRecherche[6]; ?></a> <a class="rang-name "><?php echo $_GET["joueur"] ?></a> <a class="rang-score"><?php echo $scoreRecherche[6]; ?></a>
			</div>
			<?php } ?>

		</div>

	</div>
	</body>
</html>
