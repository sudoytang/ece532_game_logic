/*
 * gameplaystate.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_GAMEPLAYSTATE_HPP_
#define SRC_GAME_GAMEPLAYSTATE_HPP_


struct GamePlayState {
	static constexpr int TOTAL_LAPS = 3;
	enum State {
		BEFORE_START,
		START,
		HALFWAY,
		END,
	} state = BEFORE_START;
	int lap = 0;
	void onHitFinishLine() {

		if (state == BEFORE_START) {
			xil_printf("GamePlay State Update: lap = %d, state = %s\n", lap, "START");
			state = START;
		} else if (state == HALFWAY) {
			if (lap == TOTAL_LAPS - 1) {
				xil_printf("GamePlay State Update: lap = %d, state = %s\n", lap, "END");
				state = END;
			} else {
				xil_printf("GamePlay State Update: lap = %d, state = %s\n", lap, "START");
				state = START;
				lap++;
			}
		}
	}
	void onHitHalfwayLine() {
		if (state == START) {
			state = HALFWAY;
			xil_printf("GamePlay State Update: lap = %d, state = %s\n", lap, "HALFWAY");
		}
	}
};


#endif /* SRC_GAME_GAMEPLAYSTATE_HPP_ */
