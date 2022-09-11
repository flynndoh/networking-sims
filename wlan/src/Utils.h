#ifndef UTILS_H_
#define UTILS_H_

// Helper to add the filename, function name and line number to EV log statements
#define HERE __FILE__ << "::" << __func__ << ":" << __LINE__ << " (" << getFullName() << "):"

#endif /* UTILS_H_ */
