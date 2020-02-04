void evenement()
{
    if(!dead){

				SDL_Event ev;
				while ( SDL_PollEvent(&ev) )
				{
					if (ev.type == SDL_KEYDOWN){

						if(ev.key.keysym.sym == SDLK_SPACE)
						{
							Mix_PlayChannel( -1, flap_wav,0);
							upper = UPPER_STEP;
							gravity_speed = 0;
						}
						else if (ev.key.keysym.sym == SDLK_ESCAPE)
						{
							SDL_Delay(5000);
						}

				}

			}
			

			if (upper>0){
				coordonner_perso.y -= ( UPPER_BY_STEP - (upper -= UPPER_SPEED) ) / (FPS/30);
			} else {
				acceleration = 0;
				coordonner_perso.y += ( gravity_speed += GRAVITY_SPEED ) / (FPS/30);
			}
}