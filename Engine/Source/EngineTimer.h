class EngineTimer {
public:
	EngineTimer();
	~EngineTimer();
	void Start();
	float Read();
	float Stop();
private:
	unsigned long long counter;
	unsigned long long perfFrequency;
	float stopTime;
	bool stop;
};