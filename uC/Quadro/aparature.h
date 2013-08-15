void aparature_init();
void EXTI15_10_IRQHandler();

typedef struct {
	//! 0-170
	int32_t throttle_up_down;
	int32_t throttle_left_right; //! 0-170
	int32_t dir_up_down; // 0-170
	int32_t dir_left_right; //0 - 170
	int32_t left_trigger; //1 0
	int32_t right_trigger; //2 1 0
	int32_t aux1; // 1 0
	int32_t aux2; // 1 0

} PILOTState;

PILOTState pilot;
