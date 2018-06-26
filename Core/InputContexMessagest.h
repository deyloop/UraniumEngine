#pragma once
struct LoadContextFile {
	const char* filename;
};

struct ActivateContext {
	const char* contextName;
};

struct DeactivateContext {
	const char* contextName;
};