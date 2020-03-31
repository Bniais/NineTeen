//src : https://tfeysal.wordpress.com/2011/01/10/programmer-un-tetris-en-c-partie-1/

#define NB_SIZES 2
#define NB_PIECES 7
#define PIECE_SIZE 4
#define NB_ROTA 4

#define RATIO_GIANT 2

const unsigned int PIECES[NB_SIZES][NB_PIECES][NB_ROTA][PIECE_SIZE * RATIO_GIANT][PIECE_SIZE * RATIO_GIANT] =
{
	{
	    { // o
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 1, 2, 0, 0, 0, 0, 0},
	            {0, 1, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	 		{
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 2, 1, 0, 0, 0, 0, 0},
	            {0, 1, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 1, 1, 0, 0, 0, 0, 0},
	            {0, 2, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 1, 1, 0, 0, 0, 0, 0},
	            {0, 1, 2, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        }
	    },

	    { // i
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {1, 1, 2, 1, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 2, 0, 0, 0, 0, 0},
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {1, 2, 1, 1, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 2, 0, 0, 0, 0, 0},
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        }
	    },
	    { // s
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 2, 1, 0, 0, 0, 0},
	            {0, 1, 1, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 2, 1, 0, 0, 0, 0},
	            {0, 0, 0, 1, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 1, 1, 0, 0, 0, 0},
	            {0, 1, 2, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        },
	        {
	            {0, 0, 1, 0, 0, 0, 0, 0},
	            {0, 0, 1, 2, 0, 0, 0, 0},
	            {0, 0, 0, 1, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0},
	            {0, 0, 0, 0, 0, 0, 0, 0}
	        }
	    },

		{ // z
		        {
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 1, 2, 0, 0, 0, 0, 0},
		            {0, 0, 1, 1, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0}
		        },
		        {
		            {0, 0, 0, 1, 0, 0, 0, 0},
		            {0, 0, 2, 1, 0, 0, 0, 0},
		            {0, 0, 1, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0}
		        },
		        {
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 1, 1, 0, 0, 0, 0, 0},
		            {0, 0, 2, 1, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0}
		        },
		        {
		            {0, 0, 0, 1, 0, 0, 0, 0},
		            {0, 0, 1, 2, 0, 0, 0, 0},
		            {0, 0, 1, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0},
		            {0, 0, 0, 0, 0, 0, 0, 0}
		        }
		},
		{ // l
		   {
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 1, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 2, 0, 0, 0, 0, 0},
			   {0, 0, 1, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 0, 1, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 1, 1, 0, 0, 0, 0, 0},
			   {0, 0, 2, 0, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   }
	   },

	   { // j
		   {
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 0, 0, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 1, 0, 0, 0, 0},
			   {0, 0, 2, 0, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 1, 0, 0, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 2, 0, 0, 0, 0, 0},
			   {0, 1, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   }
	   },

	   { // t
		   {
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 2, 1, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 1, 2, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   },
		   {
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 1, 2, 0, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0},
			   {0, 0, 0, 0, 0, 0, 0, 0}
		   }
	   }
   },
   {
	   { // O
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 2, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	    		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 2, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 2, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 2, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
	   	},

	   	{ // I
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{1, 1, 1, 1, 2, 1, 1, 1},
	   			{1, 1, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 2, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{1, 1, 1, 1, 1, 1, 1, 1},
	   			{1, 1, 1, 2, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 2, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0}
	   		}
	   	},
	   	{ // S
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}

	   		},
	    		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 2, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}

	   		},
	    		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 2},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
	   	},

	   	{ // Z
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 2, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 2},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
	   	},

	   	{ // L
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 2, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 2, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 2, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 2, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
	   	},

	   	{ // J
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 2, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 2, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 1, 1, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 2, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
	   	},

	   	{ // T
	   		{
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 1, 1, 2, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 1, 1},
	   			{0, 0, 0, 0, 2, 1, 1, 1},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 1, 1},
	   			{0, 0, 1, 1, 1, 2, 1, 1},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		},
	   		{
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 1, 1, 1, 2, 0, 0},
	   			{0, 0, 1, 1, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 1, 1, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0},
	   			{0, 0, 0, 0, 0, 0, 0, 0}
	   		}
		}
   	}
};
