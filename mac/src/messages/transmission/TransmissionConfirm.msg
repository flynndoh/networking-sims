cplusplus {{
#include "TransmissionConfirmStatus_m.h"
}}

enum TransmissionConfirmStatus;

message TransmissionConfirm {
	int status @enum(TransmissionConfirmStatus);
}