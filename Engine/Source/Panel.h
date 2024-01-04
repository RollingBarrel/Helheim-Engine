#pragma once
class Panel
{
public:
	Panel();
	Panel(const char* name, bool open = true);
	virtual ~Panel();

	virtual void Draw(int windowFlags = 0);
	virtual void Open() { mOpen = true; }
	virtual void Close() { mOpen = false; }

	bool IsOpen() const;

	const char* GetName() const { return mName; }

protected:
	bool mOpen;

private:
	const char* mName = nullptr;
};