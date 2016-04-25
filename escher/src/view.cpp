extern "C" {
#include <assert.h>
}
#include <escher/view.h>

View::View(KDRect frame) :
  m_frame(frame),
  m_superview(nullptr)
{
  for (uint8_t i=0; i<k_maxNumberOfSubviews; i++) {
    m_subviews[i] = nullptr;
  }
}

View::~View() {
}

void View::drawRect(KDRect rect) {
  // By default, a view doesn't do anything
  // It's transparent!
}

void View::redraw() {
  redraw(bounds());
}

void View::redraw(KDRect rect) {
  // Fisrt, let's draw our own content by calling drawRect
  KDSetOrigin(absoluteOrigin());
  this->drawRect(rect);
  // Then, let's recursively draw our children over ourself
  for (uint8_t i=0; i<k_maxNumberOfSubviews; i++) {
    View * subview = m_subviews[i];
    if (subview == nullptr) {
      continue;
    }
    KDRect intersection = KDRectIntersect(rect, subview->m_frame);
    if (intersection.width > 0 || intersection.height > 0) {
      // Let's express intersection in subview's coordinates
      intersection.x -= subview->m_frame.x;
      intersection.y -= subview->m_frame.y;
      subview->redraw(intersection);
    }
  }
}

void View::addSubview(View * subview) {
  // Let's find a spot for that subview
  uint8_t i = 0;
  while (m_subviews[i] != nullptr) {
    i++;
    assert(i<k_maxNumberOfSubviews); // No room left!
  }

  subview->m_superview = this;
  m_subviews[i] = subview;

  // That subview needs to be drawn
  subview->redraw();
}

void View::removeFromSuperview() {
  assert(m_superview != nullptr);
  // First, remove the view from its parent hierarchy
  assert(false); // FIXME: Unimplemented
  // Then, redraw the parent
  m_superview->drawRect(m_frame);
}

void View::setFrame(KDRect frame) {
 // TODO: Return if frame is equal to m_frame
 KDRect previousFrame = m_frame;
 m_frame = frame;
 if (m_superview != nullptr) {
   /* We have moved this view. This left a blank spot in its superview were it
    * previously was. So let's redraw that part of the superview. */
   m_superview->redraw(previousFrame);
 }
 // The view now needs to redraw itself entirely
 redraw();
}

KDRect View::bounds() {
  KDRect bounds = m_frame;
  bounds.x = 0;
  bounds.y = 0;
  return bounds;
}

KDPoint View::absoluteOrigin() {
  if (m_superview == nullptr) {
    return m_frame.origin;
  } else {
    KDPoint parentOrigin = m_superview->absoluteOrigin();
    KDPoint myOrigin = m_frame.origin;
    KDPoint result;
    result.x = parentOrigin.x + myOrigin.x;
    result.y = parentOrigin.y + myOrigin.y;
    return result;
  }
}