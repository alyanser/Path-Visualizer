#pragma once

#include <QStackedWidget>
#include <QLabel>
#include "pushButton.h"

class QHBoxLayout;

class StackedWidget : public QStackedWidget {
	Q_OBJECT
	enum class PagePosition {
		Starting,
		Middle,
		Ending
	};
	enum Dims {
		Width = 800,
		Height = 400
	};

public:
	explicit StackedWidget(QSize windowSize, QWidget * parent = nullptr);
	StackedWidget(const StackedWidget & other) = delete;
	StackedWidget(StackedWidget && other) = delete;
	StackedWidget & operator=(const StackedWidget & other) = delete;
	StackedWidget & operator=(StackedWidget && other) = delete;

private:
	[[nodiscard]]
	QWidget * populateDefinitionPage() noexcept;
	[[nodiscard]]
	QWidget * populateBlockGifPage() noexcept;
	[[nodiscard]]
	QWidget * populateNodeDragPage() noexcept;
	[[nodiscard]]
	QWidget * populateDistancePage() noexcept;
	[[nodiscard]]
	QWidget * populateTabShiftPage() noexcept;
	[[nodiscard]]
	QWidget * populateSpeedPage() noexcept;
	[[nodiscard]]
	QWidget * populateUpdateTab() noexcept;

	[[nodiscard]]
	QHBoxLayout * getBottomLayout(QWidget * parentWidget, PagePosition position) noexcept;
	[[nodiscard]]
	PushButton * getNewNextButton(QWidget * parentWidget) noexcept;
	[[nodiscard]]
	PushButton * getNewCloseButton(QWidget * parentWidget) noexcept;
	[[nodiscard]]
	PushButton * getNewPrevButton(QWidget * parentWidget) noexcept;
	[[nodiscard]]
	QLabel * getNewLabel(QWidget * parentWidget) const noexcept;

	void configureGeometry(QSize windowSize) noexcept;
	void connectWithWidgetClose(PushButton * button) noexcept;
};

inline PushButton * StackedWidget::getNewPrevButton(QWidget * parentWidget) noexcept {
	auto * button = new PushButton("Prev", parentWidget);
	button->setToolTip("Go to previous page");

	connect(button, &PushButton::clicked, [this] { setCurrentIndex(currentIndex() - 1); });

	return button;
}

inline QLabel * StackedWidget::getNewLabel(QWidget * parentWidget) const noexcept {
	auto * label = new QLabel(parentWidget);
	label->setWordWrap(true);
	label->setAlignment(Qt::AlignCenter);
	return label;
}

inline PushButton * StackedWidget::getNewNextButton(QWidget * parentWidget) noexcept {
	auto * button = new PushButton("Next", parentWidget);
	button->setToolTip("Go to next page");

	connect(button, &PushButton::clicked, [this] { setCurrentIndex(currentIndex() + 1); });

	return button;
}

inline PushButton * StackedWidget::getNewCloseButton(QWidget * parentWidget) noexcept {
	auto * button = new PushButton("Close", parentWidget);
	button->setToolTip("Close this help menu");
	connectWithWidgetClose(button);
	return button;
}

inline void StackedWidget::connectWithWidgetClose(PushButton * button) noexcept {
	connect(button, &QPushButton::clicked, [this] {
		setCurrentIndex(0);
		close();
	});
}

inline void StackedWidget::configureGeometry(const QSize windowSize) noexcept {
	const int32_t xCord = windowSize.width() / 2;
	const int32_t yCord = windowSize.height() / 2;
	setGeometry(xCord - static_cast<int>(Width) / 2, yCord - static_cast<int>(Height) / 2, Width, Height);
	setFixedSize(Width, Height);
}