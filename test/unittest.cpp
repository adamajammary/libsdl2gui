#if defined _windows && defined _DEBUG

#include <windows.h>
#include <CppUnitTest.h>
#include <libsdl2gui.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LSG_UnitTest
{
    std::string WorkingDir = "";

    TEST_MODULE_INITIALIZE(Start)
    {
        try
        {
            HMODULE      hmodule;
            static TCHAR moduleName;
            TCHAR        pathA[MAX_PATH + 1];

            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &moduleName, &hmodule);
            GetModuleFileNameA(hmodule, pathA, (MAX_PATH + 1));

            auto path = std::string(pathA);

            WorkingDir = path.substr(0, path.rfind("\\") + 1);

            LSG_StartTest("ui/main.xml", WorkingDir);

            Assert::IsTrue(LSG_IsRunning());
        }
        catch (const std::exception& e)
        {
            Assert::Fail(ToString(e.what()).c_str());
        }
    }

    TEST_MODULE_CLEANUP(Quit)
    {
        LSG_Quit();
    }

    TEST_CLASS(Cards)
    {
        TEST_METHOD(AddCard)
        {
            try
            {
                SetCards();

                LSG_CardItem card = {
                    .title       = "Card #5",
                    .description = "The fifth card.",
                    .thumbnail   = "/path/to/image/file5.jpg"
                };

                LSG_AddCard("Cards", card);

                auto cards = LSG_GetCards("Cards");

                Assert::AreEqual(5, (int)cards.size());

                auto card4 = LSG_GetCard("Cards", 4);

                Assert::AreEqual(card.title.c_str(),       card4.title.c_str());
                Assert::AreEqual(card.description.c_str(), card4.description.c_str());
                Assert::AreEqual(card.thumbnail.c_str(),   card4.thumbnail.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetCard)
        {
            try
            {
                SetCards();

                auto cardN1 = LSG_GetCard("Cards", -1);
                auto card1  = LSG_GetCard("Cards", 0);
                auto card2  = LSG_GetCard("Cards", 1);
                auto card3  = LSG_GetCard("Cards", 2);
                auto card4  = LSG_GetCard("Cards", 3);

                Assert::IsTrue(cardN1.title.empty());
                Assert::IsTrue(cardN1.description.empty());
                Assert::IsTrue(cardN1.thumbnail.empty());

                Assert::IsTrue(card1.title.empty());
                Assert::IsTrue(card1.description.empty());
                Assert::IsTrue(card1.thumbnail.empty());

                Assert::AreEqual("Card #2", card2.title.c_str());
                Assert::IsTrue(card2.description.empty());
                Assert::IsTrue(card2.thumbnail.empty());

                Assert::AreEqual("Card #3", card3.title.c_str());
                Assert::IsTrue(card3.description.empty());
                Assert::AreEqual("/path/to/image/file3.jpg", card3.thumbnail.c_str());

                Assert::AreEqual("Card #4", card4.title.c_str());
                Assert::AreEqual("The fourth card.", card4.description.c_str());
                Assert::AreEqual("/path/to/image/file4.jpg", card4.thumbnail.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetCards)
        {
            try
            {
                SetCards();

                auto cards = LSG_GetCards("Cards");

                Assert::AreEqual(4, (int)cards.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveCard)
        {
            try
            {
                SetCards();

                LSG_RemoveCard("Cards", -1);
                LSG_RemoveCard("Cards", 0);
                LSG_RemoveCard("Cards", 3);

                auto card1 = LSG_GetCard("Cards", 0);
                auto card2 = LSG_GetCard("Cards", 1);

                Assert::AreEqual("Card #2", card1.title.c_str());
                Assert::IsTrue(card1.description.empty());
                Assert::IsTrue(card1.thumbnail.empty());

                Assert::AreEqual("Card #3", card2.title.c_str());
                Assert::IsTrue(card2.description.empty());
                Assert::AreEqual("/path/to/image/file3.jpg", card2.thumbnail.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SelectCard)
        {
            try
            {
                SetCards();

                LSG_SelectCards("Cards", { -1, 1, 3, 4 });

                auto cards = LSG_GetSelectedCards("Cards");

                Assert::AreEqual(2, (int)cards.size());
                Assert::AreEqual(1, (int)cards[0]);
                Assert::AreEqual(3, (int)cards[1]);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetCard)
        {
            try
            {
                SetCards();

                LSG_CardItem cardItem = {
                    .title       = "Card #2a",
                    .description = "The updated second card.",
                    .thumbnail   = "/path/to/image/file2a.jpg"
                };

                LSG_SetCard("Cards", -1, cardItem);

                auto card = LSG_GetCard("Cards", -1);

                Assert::IsTrue(card.title.empty());
                Assert::IsTrue(card.description.empty());
                Assert::IsTrue(card.thumbnail.empty());

                LSG_SetCard("Cards", 0, cardItem);

                card = LSG_GetCard("Cards", 0);

                Assert::AreEqual(cardItem.title,       card.title);
                Assert::AreEqual(cardItem.description, card.description);
                Assert::AreEqual(cardItem.thumbnail,   card.thumbnail);

                LSG_SetCard("Cards", 4, cardItem);

                card = LSG_GetCard("Cards", 4);

                Assert::IsTrue(card.title.empty());
                Assert::IsTrue(card.description.empty());
                Assert::IsTrue(card.thumbnail.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetCards)
        {
            try
            {
                LSG_CardItems cardItems = {
                    { .title = "", .description = "", .thumbnail = ""},
                    { .title = "Card #2", .description = "", .thumbnail = "" },
                    { .title = "Card #3", .description = "", .thumbnail = "/path/to/image/file3.jpg" },
                    { .title = "Card #4", .description = "The fourth card.", .thumbnail = "/path/to/image/file4.jpg" }
                };

                LSG_SetCards("Cards", cardItems);

                auto cards = LSG_GetCards("Cards");

                Assert::AreEqual(cardItems.size(), cards.size());

                for (size_t i = 0; i < cards.size(); i++) {
                    Assert::AreEqual(cardItems[i].title,       cards[i].title);
                    Assert::AreEqual(cardItems[i].description, cards[i].description);
                    Assert::AreEqual(cardItems[i].thumbnail,   cards[i].thumbnail);
                }
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

	TEST_CLASS(List)
	{
        TEST_METHOD(AddItem)
        {
            try
            {
                for (int i = 0; i < 200; i++)
                    LSG_AddListItem("List", "My new list item number " + std::to_string(i));

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(212,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetItem)
        {
            try
            {
                SetItems();

                auto itemN1  = LSG_GetListItem("List", -1);
                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::IsTrue(itemN1.empty());
                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());

                auto pageItemN1  = LSG_GetPageListItem("List", -1);
                auto pageItem0   = LSG_GetPageListItem("List", 0);
                auto pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::IsTrue(pageItemN1.empty());
                Assert::AreEqual("My new list item number 0", pageItem0.c_str());
                Assert::IsTrue(pageItem199.empty());

                LSG_SetPage("List", 1);

                item0   = LSG_GetListItem("List", 0);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());

                pageItem0   = LSG_GetPageListItem("List", 0);
                pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::AreEqual("My new list item number 189", pageItem0.c_str());
                Assert::IsTrue(pageItem199.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetItems)
        {
            try
            {
                SetItems();

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(200,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveItem)
        {
            try
            {
                SetItems();

                LSG_RemoveListItem("List", -1);
                LSG_RemoveListItem("List", 0);
                LSG_RemoveListItem("List", 199);

                auto item0   = LSG_GetListItem("List", 0);
                auto item197 = LSG_GetListItem("List", 197);
                auto item198 = LSG_GetListItem("List", 198);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 1",  item0.c_str());
                Assert::AreEqual("My new list item number 98", item197.c_str());
                Assert::AreEqual("My new list item number 99", item198.c_str());
                Assert::IsTrue(item199.empty());

                LSG_RemovePageListItem("List", -1);
                LSG_RemovePageListItem("List", 0);
                LSG_RemovePageListItem("List", 198);

                LSG_SetPage("List", 1);

                auto pageItem0 = LSG_GetPageListItem("List", 0);

                Assert::AreEqual("My new list item number 190", pageItem0.c_str());

                item0   = LSG_GetListItem("List", 0);
                item197 = LSG_GetListItem("List", 197);
                item198 = LSG_GetListItem("List", 198);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 10", item0.c_str());
                Assert::AreEqual("My new list item number 99", item197.c_str());
                Assert::IsTrue(item198.empty());
                Assert::IsTrue(item199.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SelectRow)
        {
            try
            {
                SetItems();

                auto rows1 = LSG_GetSelectedRows("List");

                Assert::AreEqual(1, (int)rows1.size());
                Assert::AreEqual(0, (int)rows1[0]);

                LSG_SelectRow("List", 1);

                auto rows2 = LSG_GetSelectedRows("List");

                Assert::AreEqual(1, (int)rows2.size());
                Assert::AreEqual(1, (int)rows2[0]);

                LSG_SelectRows("List", { -1, 1, 3, 100 });

                auto rows3 = LSG_GetSelectedRows("List");

                Assert::AreEqual(2, (int)rows3.size());
                Assert::AreEqual(1, (int)rows3[0]);
                Assert::AreEqual(3, (int)rows3[1]);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetItem)
        {
            try
            {
                SetItems();

                LSG_SetListItem("List", 0,   "A - The first item!");
                LSG_SetListItem("List", 199, "Z - The last item!");

                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("A - The first item!", item0.c_str());
                Assert::AreEqual("Z - The last item!",  item199.c_str());

                LSG_SetPageListItem("List", 0,   "A - The first page item!");
                LSG_SetPageListItem("List", 199, "Z - The last page item!");

                auto pageItem0   = LSG_GetPageListItem("List", 0);
                auto pageItem199 = LSG_GetPageListItem("List", 199);

                Assert::IsTrue(pageItem199.empty());

                Assert::AreEqual("A - The first page item!", pageItem0.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetItems)
        {
            try
            {
                LSG_Strings newItems = {};

                for (int i = 0; i < 200; i++)
                    newItems.push_back("My new list item number " + std::to_string(i));

                LSG_SetPage("List", 0);
                LSG_SetListItems("List", newItems);

                auto items     = LSG_GetListItems("List");
                auto pageItems = LSG_GetPageListItems("List");

                Assert::AreEqual(200,                   (int)items.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageItems.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetItems();

                LSG_SortList("List", LSG_SORT_ORDER_DESCENDING);

                auto item0   = LSG_GetListItem("List", 0);
                auto item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 99", item0.c_str());
                Assert::AreEqual("My new list item number 0",  item199.c_str());

                LSG_SortList("List", LSG_SORT_ORDER_ASCENDING);

                item0   = LSG_GetListItem("List", 0);
                item199 = LSG_GetListItem("List", 199);

                Assert::AreEqual("My new list item number 0",  item0.c_str());
                Assert::AreEqual("My new list item number 99", item199.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
	};

    TEST_CLASS(Navigation)
    {
        TEST_METHOD(ItemCount)
        {
            try
            {
                LSG_SetNavigationItemCount("Navigation", 10);

                auto itemCount = LSG_GetNavigationItemCount("Navigation");

                Assert::AreEqual(10, (int)itemCount);

                LSG_SetNavigationItemCount("Navigation", 20);

                itemCount = LSG_GetNavigationItemCount("Navigation");

                Assert::AreEqual(20, (int)itemCount);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Navigate)
        {
            try
            {
                LSG_SetNavigationItemCount("Navigation", 20, 1);

                LSG_NavigateEnd("Navigation");

                auto position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(19, position);

                LSG_NavigateBack("Navigation");

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(18, position);

                LSG_NavigateHome("Navigation");

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(0, position);

                LSG_NavigateForward("Navigation");

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(1, position);

                LSG_NavigateTo("Navigation", 5);

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(5, position);

                LSG_NavigateTo("Navigation", -1);

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(0, position);

                LSG_NavigateTo("Navigation", 20);

                position = LSG_GetNavigationPosition("Navigation");

                Assert::AreEqual(19, position);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(Slider)
    {
        TEST_METHOD(GetParts)
        {
            try
            {
                auto parts = LSG_GetSliderParts("Slider");

                Assert::AreEqual(3, (int)parts.size());

                Assert::AreEqual(0.25, parts[0]);
                Assert::AreEqual(0.5,  parts[1]);
                Assert::AreEqual(0.75, parts[2]);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetValue)
        {
            try
            {
                auto value = LSG_GetSliderValue("Slider");

                Assert::AreEqual(0.5, value);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetParts)
        {
            try
            {
                LSG_SetSliderParts("Slider", { 0.1, 0.2, 0.6, 0.8 });

                auto parts = LSG_GetSliderParts("Slider");

                Assert::AreEqual(4, (int)parts.size());

                Assert::AreEqual(0.1, parts[0]);
                Assert::AreEqual(0.2, parts[1]);
                Assert::AreEqual(0.6, parts[2]);
                Assert::AreEqual(0.8, parts[3]);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetValue)
        {
            try
            {
                LSG_SetSliderValue("Slider", 0.1);

                auto value = LSG_GetSliderValue("Slider");

                Assert::AreEqual(0.1, value);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

	TEST_CLASS(TableHeader)
	{
        TEST_METHOD(GetHeader)
        {
            try
            {
                auto header = LSG_GetTableHeader("TableWithGroups");

                Assert::AreEqual(2, (int)header.size());

                Assert::AreEqual("Dolor", header[0].c_str());
                Assert::AreEqual("Magna", header[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveHeader)
        {
            try
            {
                LSG_RemoveTableHeader("TableWithGroups");

                auto header = LSG_GetTableHeader("TableWithGroups");

                Assert::IsTrue(header.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetHeader)
        {
            try
            {
                LSG_SetTableHeader("TableWithGroups", { "New Header A", "New Header B", "New Header C" });

                auto header = LSG_GetTableHeader("TableWithGroups");

                Assert::AreEqual(3, (int)header.size());

                Assert::AreEqual("New Header A", header[0].c_str());
                Assert::AreEqual("New Header B", header[1].c_str());
                Assert::AreEqual("New Header C", header[2].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TableGroup)
    {
        TEST_METHOD(AddGroup)
        {
            try
            {
                for (int i = 0; i < 20; i++)
                {
                    LSG_TableGroup newGroup = { .group = ("My new group " + std::to_string(i)) };

                    for (int j = 0; j < 10; j++)
                        newGroup.rows.push_back({ ("My new group row " + std::to_string(j) + " - Column A"), ("My new group row " + std::to_string(j) + " - Column B") });

                    LSG_AddTableGroup("TableWithGroups", newGroup);
                }

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(22, (int)groups.size());
                Assert::AreEqual(10, (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetGroup)
        {
            try
            {
                SetGroups();

                auto groupN1 = LSG_GetTableGroup("TableWithGroups", "My new group -1");
                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(groupN1.group.empty());
                Assert::IsTrue(groupN1.rows.empty());

                Assert::AreEqual(10, (int)group0.rows.size());
                Assert::AreEqual(10, (int)group19.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[0][1].c_str());

                Assert::AreEqual("My new group 19",               group19.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group19.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group19.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetGroups)
        {
            try
            {
                SetGroups();

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(20, (int)groups.size());
                Assert::AreEqual(9,  (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveGroup)
        {
            try
            {
                SetGroups();

                LSG_RemoveTableGroup("TableWithGroups", "My new group -1");
                LSG_RemoveTableGroup("TableWithGroups", "My new group 0");
                LSG_RemoveTableGroup("TableWithGroups", "My new group 19");

                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group17 = LSG_GetTableGroup("TableWithGroups", "My new group 17");
                auto group18 = LSG_GetTableGroup("TableWithGroups", "My new group 18");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(group0.group.empty());
                Assert::IsTrue(group0.rows.empty());

                Assert::IsTrue(group19.group.empty());
                Assert::IsTrue(group19.rows.empty());

                Assert::AreEqual(10, (int)group17.rows.size());
                Assert::AreEqual(10, (int)group18.rows.size());

                Assert::AreEqual("My new group 17",               group17.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group17.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group17.rows[0][1].c_str());

                Assert::AreEqual("My new group 18",               group18.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group18.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group18.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetGroup)
        {
            try
            {
                SetGroups();

                LSG_SetTableGroup("TableWithGroups", { .group = "My new group -1", .rows = {{ ("My non-existing group row - Column A"), ("My non-existing group row - Column B") }} });
                LSG_SetTableGroup("TableWithGroups", { .group = "My new group 0",  .rows = {{ ("My first group row - Column A"),        ("My first group row - Column B") }} });
                LSG_SetTableGroup("TableWithGroups", { .group = "My new group 19", .rows = {{ ("My last group row - Column A"),         ("My last group row - Column B") }} });

                auto groupN1 = LSG_GetTableGroup("TableWithGroups", "My new group -1");
                auto group0  = LSG_GetTableGroup("TableWithGroups", "My new group 0");
                auto group19 = LSG_GetTableGroup("TableWithGroups", "My new group 19");

                Assert::IsTrue(groupN1.group.empty());
                Assert::IsTrue(groupN1.rows.empty());

                Assert::AreEqual(1, (int)group0.rows.size());
                Assert::AreEqual(1, (int)group19.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My first group row - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My first group row - Column B", group0.rows[0][1].c_str());

                Assert::AreEqual("My new group 19",              group19.group.c_str());
                Assert::AreEqual("My last group row - Column A", group19.rows[0][0].c_str());
                Assert::AreEqual("My last group row - Column B", group19.rows[0][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetGroups)
        {
            try
            {
                LSG_TableGroups newGroups = {};

                for (int i = 0; i < 20; i++)
                {
                    LSG_TableGroup newGroup = { .group = ("My new group " + std::to_string(i)) };

                    for (int j = 0; j < 10; j++)
                        newGroup.rows.push_back({ ("My new group row " + std::to_string(j) + " - Column A"), ("My new group row " + std::to_string(j) + " - Column B") });

                    newGroups.push_back(newGroup);
                }

                LSG_SetPage("TableWithGroups", 0);
                LSG_SetTableGroups("TableWithGroups", newGroups);

                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(20, (int)groups.size());
                Assert::AreEqual(9,  (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetGroups();

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_DESCENDING, 1);

                auto group0 = LSG_GetTableGroup("TableWithGroups", "My new group 0");

                Assert::AreEqual(10, (int)group0.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 9 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group0.rows[0][1].c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[9][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[9][1].c_str());

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_ASCENDING, 1);

                group0 = LSG_GetTableGroup("TableWithGroups", "My new group 0");

                Assert::AreEqual(10, (int)group0.rows.size());

                Assert::AreEqual("My new group 0",                group0.group.c_str());
                Assert::AreEqual("My new group row 0 - Column A", group0.rows[0][0].c_str());
                Assert::AreEqual("My new group row 0 - Column B", group0.rows[0][1].c_str());
                Assert::AreEqual("My new group row 9 - Column A", group0.rows[9][0].c_str());
                Assert::AreEqual("My new group row 9 - Column B", group0.rows[9][1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TableRow)
	{
        TEST_METHOD(AddRow)
        {
            try
            {
                SetRows();

                LSG_AddTableRow("TableWithGroups", { ("My new table row 200 - Column A"), ("My new table row 200 - Column B") });

                auto rows     = LSG_GetTableRows("TableWithGroups");
                auto pageRows = LSG_GetPageTableRows("TableWithGroups");

                Assert::AreEqual(201,                   (int)rows.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageRows.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetRow)
        {
            try
            {
                SetRows();

                auto rowN1  = LSG_GetTableRow("TableWithGroups", -1);
                auto row0   = LSG_GetTableRow("TableWithGroups", 0);
                auto row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::IsTrue(rowN1.empty());

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());

                auto pageRowN1  = LSG_GetPageTableRow("TableWithGroups", -1);
                auto pageRow0   = LSG_GetPageTableRow("TableWithGroups", 0);
                auto pageRow199 = LSG_GetPageTableRow("TableWithGroups", 199);

                Assert::IsTrue(pageRowN1.empty());
                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 0 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", pageRow0[1].c_str());

                LSG_SetPage("TableWithGroups", 1);

                row0   = LSG_GetTableRow("TableWithGroups", 0);
                row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());

                pageRow0   = LSG_GetPageTableRow("TableWithGroups", 0);
                pageRow199 = LSG_GetPageTableRow("TableWithGroups", 199);

                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 189 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 189 - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetRows)
        {
            try
            {
                SetRows();

                auto rows       = LSG_GetTableRows("TableWithGroups");
                auto groups     = LSG_GetTableGroups("TableWithGroups");
                auto pageRows   = LSG_GetPageTableRows("TableWithGroups");
                auto pageGroups = LSG_GetPageTableGroups("TableWithGroups");

                Assert::AreEqual(200,                   (int)rows.size());
                Assert::AreEqual(LSG_MAX_ROWS_PER_PAGE, (int)pageRows.size());
                Assert::AreEqual(0,                     (int)groups.size());
                Assert::AreEqual(0,                     (int)pageGroups.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveRow)
        {
            try
            {
                SetRows();

                LSG_RemoveTableRow("TableWithGroups", -1);
                LSG_RemoveTableRow("TableWithGroups", 0);
                LSG_RemoveTableRow("TableWithGroups", 199);

                auto row0   = LSG_GetTableRow("TableWithGroups", 0);
                auto row197 = LSG_GetTableRow("TableWithGroups", 197);
                auto row198 = LSG_GetTableRow("TableWithGroups", 198);
                auto row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::IsTrue(row199.empty());

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row197.size());
                Assert::AreEqual(2, (int)row198.size());

                Assert::AreEqual("My new table row 1 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 1 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 98 - Column A", row197[0].c_str());
                Assert::AreEqual("My new table row 98 - Column B", row197[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row198[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row198[1].c_str());

                LSG_RemovePageTableRow("TableWithGroups", -1);
                LSG_RemovePageTableRow("TableWithGroups", 0);
                LSG_RemovePageTableRow("TableWithGroups", 198);

                auto pageRow0 = LSG_GetPageTableRow("TableWithGroups", 0);

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 10 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 10 - Column B", pageRow0[1].c_str());

                LSG_SetPage("TableWithGroups", 1);

                row0 = LSG_GetTableRow("TableWithGroups", 0);

                Assert::AreEqual(2, (int)row0.size());

                Assert::AreEqual("My new table row 10 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 10 - Column B", row0[1].c_str());

                pageRow0 = LSG_GetPageTableRow("TableWithGroups", 0);

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("My new table row 190 - Column A", pageRow0[0].c_str());
                Assert::AreEqual("My new table row 190 - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetRow)
        {
            try
            {
                SetRows();

                LSG_SetTableRow("TableWithGroups",   6, { ("A - My first table row - Column A"), ("A - My first table row - Column B") });
                LSG_SetTableRow("TableWithGroups", 199, { ("Z - My last table row - Column A"),  ("Z - My last table row - Column B") });

                auto row0   = LSG_GetTableRow("TableWithGroups", 6);
                auto row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("A - My first table row - Column A", row0[0].c_str());
                Assert::AreEqual("A - My first table row - Column B", row0[1].c_str());

                Assert::AreEqual("Z - My last table row - Column A", row199[0].c_str());
                Assert::AreEqual("Z - My last table row - Column B", row199[1].c_str());

                LSG_SetPageTableRow("TableWithGroups",   6, { ("A - My first page table row - Column A"), ("A - My first page table row - Column B") });
                LSG_SetPageTableRow("TableWithGroups", 199, { ("Z - My last page table row - Column A"),  ("Z - My last page table row - Column B") });

                auto pageRow0   = LSG_GetPageTableRow("TableWithGroups", 6);
                auto pageRow199 = LSG_GetPageTableRow("TableWithGroups", 199);

                Assert::IsTrue(pageRow199.empty());

                Assert::AreEqual(2, (int)pageRow0.size());

                Assert::AreEqual("A - My first page table row - Column A", pageRow0[0].c_str());
                Assert::AreEqual("A - My first page table row - Column B", pageRow0[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetRows)
        {
            try
            {
                LSG_TableRows newRows = {};

                for (int i = 0; i < 200; i++)
                    newRows.push_back({ ("My new table row " + std::to_string(i) + " - Column A"), ("My new table row " + std::to_string(i) + " - Column B") });

                LSG_SetPage("TableWithGroups", 0);
                LSG_SetTableGroups("TableWithGroups", {});
                LSG_SetTableRows("TableWithGroups", newRows);

            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(Sort)
        {
            try
            {
                SetRows();

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_DESCENDING, 1);

                auto row0   = LSG_GetTableRow("TableWithGroups", 0);
                auto row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 99 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 0 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row199[1].c_str());

                LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_ASCENDING, 1);

                row0   = LSG_GetTableRow("TableWithGroups", 0);
                row199 = LSG_GetTableRow("TableWithGroups", 199);

                Assert::AreEqual(2, (int)row0.size());
                Assert::AreEqual(2, (int)row199.size());

                Assert::AreEqual("My new table row 0 - Column A", row0[0].c_str());
                Assert::AreEqual("My new table row 0 - Column B", row0[1].c_str());

                Assert::AreEqual("My new table row 99 - Column A", row199[0].c_str());
                Assert::AreEqual("My new table row 99 - Column B", row199[1].c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(Text)
    {
        TEST_METHOD(SetText)
        {
            try
            {
                LSG_SetText("CardRow", "New test value\nwith wrap.");

                auto text = LSG_GetText("CardRow");

                Assert::AreEqual("New test value\nwith wrap.", text.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

    TEST_CLASS(TextInput)
    {
        TEST_METHOD(Clear)
        {
            try
            {
                SetValue();

                auto value1 = LSG_GetTextInputValue("TextInput");

                Assert::IsFalse(value1.empty());

                LSG_ClearTextInput("TextInput");

                auto value2 = LSG_GetTextInputValue("TextInput");

                Assert::IsTrue(value2.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetValue)
        {
            try
            {
                LSG_SetTextInputValue("TextInput", "New test value");

                auto value = LSG_GetTextInputValue("TextInput");

                Assert::AreEqual("New test value", value.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
    };

	TEST_CLASS(Tiles)
	{
        TEST_METHOD(AddTile)
        {
            try
            {
                SetTiles();

                LSG_TileItem tile = {
                  .image = "/path/to/image/file2.jpg",
                  .text  = "Image 2"
                };

                LSG_AddTile("Tiles", tile);

                auto tiles = LSG_GetTiles("Tiles");

                Assert::AreEqual(5, (int)tiles.size());

                auto tile4 = LSG_GetTile("Tiles", 4);

                Assert::AreEqual(tile.image.c_str(), tile4.image.c_str());
                Assert::AreEqual(tile.text.c_str(),  tile4.text.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetTile)
        {
            try
            {
                SetTiles();

                auto tileN1 = LSG_GetTile("Tiles", -1);
                auto tile1  = LSG_GetTile("Tiles", 1);
                auto tile3  = LSG_GetTile("Tiles", 3);
                auto tile4  = LSG_GetTile("Tiles", 4);

                Assert::IsTrue(tileN1.image.empty());
                Assert::IsTrue(tileN1.text.empty());

                Assert::IsTrue(tile1.image.empty());
                Assert::AreEqual("No image", tile1.text.c_str());

                Assert::AreEqual("/path/to/image/file1.jpg", tile3.image.c_str());
                Assert::AreEqual("Image 1",                  tile3.text.c_str());

                Assert::IsTrue(tile4.image.empty());
                Assert::IsTrue(tile4.text.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(GetTiles)
        {
            try
            {
                SetTiles();

                auto tiles = LSG_GetTiles("Tiles");

                Assert::AreEqual(4, (int)tiles.size());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(RemoveTile)
        {
            try
            {
                SetTiles();

                LSG_RemoveTile("Tiles", -1);
                LSG_RemoveTile("Tiles", 0);
                LSG_RemoveTile("Tiles", 3);

                auto tile0 = LSG_GetTile("Tiles", 0);
                auto tile2 = LSG_GetTile("Tiles", 2);

                Assert::IsTrue(tile0.image.empty());
                Assert::AreEqual("No image", tile0.text.c_str());

                Assert::AreEqual("/path/to/image/file1.jpg", tile2.image.c_str());
                Assert::AreEqual("Image 1",                  tile2.text.c_str());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SelectTile)
        {
            try
            {
                SetTiles();

                LSG_SelectTiles("Tiles", { -1, 1, 3, 4 });

                auto tiles = LSG_GetSelectedTiles("Tiles");

                Assert::AreEqual(2, (int)tiles.size());
                Assert::AreEqual(1, (int)tiles[0]);
                Assert::AreEqual(3, (int)tiles[1]);
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetTile)
        {
            try
            {
                SetTiles();

                LSG_TileItem tileItem = {
                    .image = "/path/to/image/file2.jpg",
                    .text  = "Image 2"
                };

                LSG_SetTile("Tiles", -1, tileItem);

                auto tile = LSG_GetTile("Tiles", -1);

                Assert::IsTrue(tile.image.empty());
                Assert::IsTrue(tile.text.empty());

                LSG_SetTile("Tiles", 0, tileItem);

                tile = LSG_GetTile("Tiles", 0);

                Assert::AreEqual(tileItem.image, tile.image);
                Assert::AreEqual(tileItem.text,  tile.text);

                LSG_SetTile("Tiles", 4, tileItem);

                tile = LSG_GetTile("Tiles", 4);

                Assert::IsTrue(tile.image.empty());
                Assert::IsTrue(tile.text.empty());
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }

        TEST_METHOD(SetTiles)
        {
            try
            {
                LSG_TileItems tileItems = {
                    { .image = "", .text = "" },
                    { .image = "", .text = "No image" },
                    { .image = "/path/to/image/file.jpg",  .text  = "" },
                    { .image = "/path/to/image/file1.jpg", .text  = "Image 1" }
                };

                LSG_SetTiles("Tiles", tileItems);

                auto tiles = LSG_GetTiles("Tiles");

                Assert::AreEqual(tileItems.size(), tiles.size());

                for (size_t i = 0; i < tiles.size(); i++) {
                    Assert::AreEqual(tileItems[i].image, tiles[i].image);
                    Assert::AreEqual(tileItems[i].text,  tiles[i].text);
                }
            }
            catch (const std::exception& e)
            {
                Assert::Fail(ToString(e.what()).c_str());
            }
        }
	};
}

#endif
